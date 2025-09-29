#include "UI/DAUIManager.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "UI/SubWidgets/DAInventoryWidget.h"
#include "UI/SubWidgets/DAPlayerStatusWidget.h"
#include "UI/SubWidgets/DANotificationWidget.h"
#include "UI/DAInteractionPromptWidget.h"
#include "UI/DADialogueWidget.h"
#include "UI/DAPlayerHUDWidget.h"
#include "UI/DAQuestLogWidget.h"
#include "UObject/ConstructorHelpers.h"

UDAUIManager::UDAUIManager()
{
}

#include "Core/KnowledgeSubsystem.h"
#include "Core/WorldEpochSubsystem.h"
#include "Core/CultureSubsystem.h"
#include "Core/WorldPersistenceSystem.h"
#include "Core/QuestManagementSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UDAUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    // Subsystem initialization - bind to other subsystems
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        if (UKnowledgeSubsystem* KnowledgeSubsystem = GameInstance->GetSubsystem<UKnowledgeSubsystem>())
        {
            KnowledgeSubsystem->OnPrincipleDiscovered.AddDynamic(this, &UDAUIManager::HandlePrincipleDiscovered);
        }
        if (UWorldEpochSubsystem* EpochSubsystem = GameInstance->GetSubsystem<UWorldEpochSubsystem>())
        {
            EpochSubsystem->OnEpochAdvanced.AddDynamic(this, &UDAUIManager::HandleEpochAdvanced);
        }
        if (UCultureSubsystem* CultureSubsystem = GameInstance->GetSubsystem<UCultureSubsystem>())
        {
            CultureSubsystem->OnCultureEvolved.AddDynamic(this, &UDAUIManager::HandleCultureEvolved);
        }
        if (UWorldPersistenceSystem* PersistenceSystem = GameInstance->GetSubsystem<UWorldPersistenceSystem>())
        {
            PersistenceSystem->OnWorldStateChanged.AddDynamic(this, &UDAUIManager::HandleWorldStateChanged);
            PersistenceSystem->OnWorldEvent.AddDynamic(this, &UDAUIManager::HandleWorldEvent);
        }
        if (UQuestManagementSubsystem* QuestSubsystem = GameInstance->GetSubsystem<UQuestManagementSubsystem>())
        {
            QuestSubsystem->OnQuestStateChanged.AddDynamic(this, &UDAUIManager::HandleQuestStateChanged);
            QuestSubsystem->OnQuestStarted.AddDynamic(this, &UDAUIManager::HandleQuestStarted);
            QuestSubsystem->OnQuestCompleted.AddDynamic(this, &UDAUIManager::HandleQuestCompleted);
            QuestSubsystem->OnQuestFailed.AddDynamic(this, &UDAUIManager::HandleQuestFailed);
            QuestSubsystem->OnQuestAbandoned.AddDynamic(this, &UDAUIManager::HandleQuestAbandoned);
        }
    }
}

void UDAUIManager::Deinitialize()
{
    Teardown();
    Super::Deinitialize();
}

bool UDAUIManager::ShouldCreateSubsystem(UObject* Outer) const
{
    return true;
}

void UDAUIManager::InitializeForPlayer(APlayerController* PlayerController)
{
    OwningPlayerController = PlayerController;
    InitializeWidgets();
}

void UDAUIManager::Teardown()
{
    if (PlayerStatusWidget)
    {
        PlayerStatusWidget->RemoveFromParent();
        PlayerStatusWidget = nullptr;
    }
    if (InventoryWidget)
    {
        InventoryWidget->RemoveFromParent();
        InventoryWidget = nullptr;
    }
    if (NotificationWidget)
    {
        NotificationWidget->RemoveFromParent();
        NotificationWidget = nullptr;
    }
    if (InteractionPromptWidget)
    {
        InteractionPromptWidget->RemoveFromParent();
        InteractionPromptWidget = nullptr;
    }
    if (DialogueWidget)
    {
        DialogueWidget->RemoveFromParent();
        DialogueWidget = nullptr;
    }
    if (QuestLogWidget)
    {
        QuestLogWidget->RemoveFromParent();
        QuestLogWidget = nullptr;
    }
    if (PlayerHUDWidget)
    {
        PlayerHUDWidget->RemoveFromParent();
        PlayerHUDWidget = nullptr;
    }
}

void UDAUIManager::InitializeWidgets()
{
    if (!OwningPlayerController)
    {
        return;
    }

    // Create the main HUD widget first
    if (PlayerHUDWidgetClass)
    {
        PlayerHUDWidget = CreateWidget<UDAPlayerHUDWidget>(OwningPlayerController, PlayerHUDWidgetClass);
        if (PlayerHUDWidget)
        {
            PlayerHUDWidget->AddToViewport();

            // Initialize sub-widgets within the main HUD
            InitializeHUDSubWidgets();
        }
    }

    // Always create standalone widgets as fallback/compatibility
    InitializeStandaloneWidgets();
}

void UDAUIManager::InitializeHUDSubWidgets()
{
    if (!PlayerHUDWidget || !OwningPlayerController)
    {
        return;
    }

    // Create and assign sub-widgets to the main HUD widget
    if (PlayerStatusWidgetClass && !PlayerHUDWidget->PlayerStatusWidget)
    {
        PlayerHUDWidget->PlayerStatusWidget = CreateWidget<UDAPlayerStatusWidget>(OwningPlayerController, PlayerStatusWidgetClass);
        if (PlayerHUDWidget->PlayerStatusWidget)
        {
            PlayerHUDWidget->PlayerStatusWidget->AddToViewport();
            // Ensure it's visible and properly positioned
            PlayerHUDWidget->PlayerStatusWidget->SetVisibility(ESlateVisibility::Visible);
        }
    }
    else if (!PlayerStatusWidgetClass)
    {
        // Try to find the default status widget class
        static ConstructorHelpers::FClassFinder<UDAPlayerStatusWidget> StatusWidgetClassFinder(TEXT("/Game/_DA/UI/WBP_PlayerStatusWidget"));
        if (StatusWidgetClassFinder.Class)
        {
            PlayerStatusWidgetClass = StatusWidgetClassFinder.Class;
            PlayerHUDWidget->PlayerStatusWidget = CreateWidget<UDAPlayerStatusWidget>(OwningPlayerController, PlayerStatusWidgetClass);
            if (PlayerHUDWidget->PlayerStatusWidget)
            {
                PlayerHUDWidget->PlayerStatusWidget->AddToViewport();
                PlayerHUDWidget->PlayerStatusWidget->SetVisibility(ESlateVisibility::Visible);
            }
        }
    }

    if (InventoryWidgetClass && !PlayerHUDWidget->InventoryWidget)
    {
        PlayerHUDWidget->InventoryWidget = CreateWidget<UDAInventoryWidget>(OwningPlayerController, InventoryWidgetClass);
    }

    if (NotificationWidgetClass && !PlayerHUDWidget->NotificationWidget)
    {
        PlayerHUDWidget->NotificationWidget = CreateWidget<UDANotificationWidget>(OwningPlayerController, NotificationWidgetClass);
    }

    if (InteractionPromptWidgetClass && !PlayerHUDWidget->InteractionPromptWidget)
    {
        PlayerHUDWidget->InteractionPromptWidget = CreateWidget<UDAInteractionPromptWidget>(OwningPlayerController, InteractionPromptWidgetClass);
    }

    if (DialogueWidgetClass && !PlayerHUDWidget->DialogueWidget)
    {
        PlayerHUDWidget->DialogueWidget = CreateWidget<UDADialogueWidget>(OwningPlayerController, DialogueWidgetClass);
    }

    if (QuestLogWidgetClass && !PlayerHUDWidget->QuestLogWidget)
    {
        PlayerHUDWidget->QuestLogWidget = CreateWidget<UDAQuestLogWidget>(OwningPlayerController, QuestLogWidgetClass);
    }

    // Keep references for backward compatibility
    PlayerStatusWidget = PlayerHUDWidget->PlayerStatusWidget;
    InventoryWidget = PlayerHUDWidget->InventoryWidget;
    NotificationWidget = PlayerHUDWidget->NotificationWidget;
    InteractionPromptWidget = PlayerHUDWidget->InteractionPromptWidget;
    DialogueWidget = PlayerHUDWidget->DialogueWidget;
    QuestLogWidget = PlayerHUDWidget->QuestLogWidget;
}

void UDAUIManager::InitializeStandaloneWidgets()
{
    // Fallback method for when no main HUD widget is used
    if (PlayerStatusWidgetClass)
    {
        PlayerStatusWidget = CreateWidget<UDAPlayerStatusWidget>(OwningPlayerController, PlayerStatusWidgetClass);
        if (PlayerStatusWidget)
        {
            PlayerStatusWidget->AddToViewport();
        }
    }

    if (InventoryWidgetClass)
    {
        InventoryWidget = CreateWidget<UDAInventoryWidget>(OwningPlayerController, InventoryWidgetClass);
    }

    if (NotificationWidgetClass)
    {
        NotificationWidget = CreateWidget<UDANotificationWidget>(OwningPlayerController, NotificationWidgetClass);
    }

    if (InteractionPromptWidgetClass)
    {
        InteractionPromptWidget = CreateWidget<UDAInteractionPromptWidget>(OwningPlayerController, InteractionPromptWidgetClass);
    }

    if (DialogueWidgetClass)
    {
        DialogueWidget = CreateWidget<UDADialogueWidget>(OwningPlayerController, DialogueWidgetClass);
    }

    if (QuestLogWidgetClass)
    {
        QuestLogWidget = CreateWidget<UDAQuestLogWidget>(OwningPlayerController, QuestLogWidgetClass);
    }
}

void UDAUIManager::ToggleInventory()
{
    if (PlayerHUDWidget)
    {
        PlayerHUDWidget->ToggleInventory();
    }
    else if (InventoryWidget)
    {
        // Fallback for standalone widget
        if (InventoryWidget->IsInViewport())
        {
            InventoryWidget->RemoveFromParent();
        }
        else
        {
            InventoryWidget->AddToViewport();
        }
    }
}

void UDAUIManager::ToggleQuestLog()
{
    if (PlayerHUDWidget)
    {
        PlayerHUDWidget->ToggleQuestLog();
    }
    else if (QuestLogWidget)
    {
        // Fallback for standalone widget
        if (QuestLogWidget->IsInViewport())
        {
            QuestLogWidget->RemoveFromParent();
        }
        else
        {
            QuestLogWidget->AddToViewport();
        }
    }
}

void UDAUIManager::ShowNotification(const FText& Message, float Duration)
{
    if (PlayerHUDWidget)
    {
        PlayerHUDWidget->ShowNotification(Message, Duration);
    }
    else if (NotificationWidget)
    {
        // Fallback for standalone widget
        FGameNotification Notification;
        Notification.Message = Message;
        Notification.Duration = Duration;
        Notification.Category = ENotificationCategory::General;
        NotificationWidget->EnqueueNotification(Notification);
    }
}

void UDAUIManager::ShowInteractionPrompt(const FText& PromptText, const FText& ActionText)
{
    if (PlayerHUDWidget)
    {
        PlayerHUDWidget->ShowInteractionPrompt(PromptText, ActionText);
    }
    else if (InteractionPromptWidget)
    {
        // Fallback for standalone widget
        InteractionPromptWidget->SetPromptText(PromptText);
        InteractionPromptWidget->SetActionText(ActionText);
        if (!InteractionPromptWidget->IsInViewport())
        {
            InteractionPromptWidget->AddToViewport();
        }
    }
}

void UDAUIManager::HideInteractionPrompt()
{
    if (PlayerHUDWidget)
    {
        PlayerHUDWidget->HideInteractionPrompt();
    }
    else if (InteractionPromptWidget && InteractionPromptWidget->IsInViewport())
    {
        // Fallback for standalone widget
        InteractionPromptWidget->RemoveFromParent();
    }
}

void UDAUIManager::ShowDialogue(const FText& SpeakerName, const FText& DialogueText, const TArray<FText>& ResponseOptions)
{
    if (PlayerHUDWidget)
    {
        PlayerHUDWidget->ShowDialogue(SpeakerName, DialogueText, ResponseOptions);
    }
    else if (DialogueWidget)
    {
        // Fallback for standalone widget
        DialogueWidget->SetDialogueData(SpeakerName, DialogueText, ResponseOptions);
        if (!DialogueWidget->IsInViewport())
        {
            DialogueWidget->AddToViewport();
        }
    }
}

void UDAUIManager::HideDialogue()
{
    if (PlayerHUDWidget)
    {
        PlayerHUDWidget->HideDialogue();
    }
    else if (DialogueWidget && DialogueWidget->IsInViewport())
    {
        // Fallback for standalone widget
        DialogueWidget->RemoveFromParent();
    }
}

void UDAUIManager::UpdatePlayerStatus(float Health, float MaxHealth, float Stamina, float MaxStamina, float Hunger, float MaxHunger, float Thirst, float MaxThirst)
{
    if (PlayerHUDWidget)
    {
        PlayerHUDWidget->UpdatePlayerStatus(Health, MaxHealth, Stamina, MaxStamina, Hunger, MaxHunger, Thirst, MaxThirst);
    }
    else if (PlayerStatusWidget)
    {
        // Fallback for standalone widget
        PlayerStatusWidget->UpdateStatus(Health, MaxHealth, Stamina, MaxStamina, Hunger, MaxHunger, Thirst, MaxThirst);
    }
}

void UDAUIManager::HandlePrincipleDiscovered(const FPrinciple& Principle)
{
    FText Message = FText::Format(FText::FromString("You discovered: {0} + {1} = {2}"),
        FText::FromName(Principle.Inputs[0]),
        FText::FromName(Principle.Process),
        FText::FromName(Principle.Result));
    ShowNotification(Message, 4.0f);
}

void UDAUIManager::HandleEpochAdvanced(const FEpochData& NewEpoch)
{
    FText Message = FText::Format(FText::FromString("A new Age has begun: {0}\n{1}"),
        FText::FromName(NewEpoch.EpochName),
        FText::FromString(NewEpoch.Description));
    ShowNotification(Message, 6.0f);
}

void UDAUIManager::HandleCultureEvolved(FName FactionID, const FCultureProfile& NewProfile)
{
    FText Message = FText::Format(FText::FromString("Culture evolved for {0}. Traits: {1}"),
        FText::FromName(FactionID),
        FText::FromString(FString::FromInt(NewProfile.Traits.Num())));
    ShowNotification(Message, 4.0f);
}

void UDAUIManager::HandleWorldStateChanged(FName RegionID, const FRegionState& NewState)
{
    FText Message = FText::Format(FText::FromString("World state changed in region: {0}"), FText::FromName(RegionID));
    ShowNotification(Message, 3.5f);
}

void UDAUIManager::HandleWorldEvent(FName EventType, FName RegionID, const FString& EventDescription)
{
    FText Message = FText::Format(FText::FromString("World Event: {0} in {1}\n{2}"),
        FText::FromName(EventType),
        FText::FromName(RegionID),
        FText::FromString(EventDescription));
    ShowNotification(Message, 5.0f);
}

void UDAUIManager::HandleQuestStateChanged(FName QuestID, EQuestState NewState)
{
    FText Message = FText::Format(FText::FromString("Quest {0} state changed: {1}"), FText::FromName(QuestID), FText::AsNumber((int32)NewState));
    ShowNotification(Message, 3.0f);
}

void UDAUIManager::HandleQuestStarted(FName QuestID)
{
    FText Message = FText::Format(FText::FromString("Quest started: {0}"), FText::FromName(QuestID));
    ShowNotification(Message, 3.0f);
}

void UDAUIManager::HandleQuestCompleted(FName QuestID)
{
    FText Message = FText::Format(FText::FromString("Quest completed: {0}"), FText::FromName(QuestID));
    ShowNotification(Message, 3.5f);
}

void UDAUIManager::HandleQuestFailed(FName QuestID)
{
    FText Message = FText::Format(FText::FromString("Quest failed: {0}"), FText::FromName(QuestID));
    ShowNotification(Message, 3.5f);
}

void UDAUIManager::HandleQuestAbandoned(FName QuestID)
{
    FText Message = FText::Format(FText::FromString("Quest abandoned: {0}"), FText::FromName(QuestID));
    ShowNotification(Message, 3.0f);
}