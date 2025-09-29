#include "UI/DAHUD.h"
#include "UI/DAUIManager.h"
#include "GameFramework/PlayerController.h"
#include "Engine/GameInstance.h"

ADAHUD::ADAHUD()
{
}

void ADAHUD::BeginPlay()
{
    Super::BeginPlay();

    // Get the UIManager subsystem from the GameInstance
    if (UWorld* World = GetWorld())
    {
        if (UGameInstance* GameInstance = World->GetGameInstance())
        {
            UIManager = GameInstance->GetSubsystem<UDAUIManager>();
            if (UIManager)
            {
                UIManager->InitializeForPlayer(GetOwningPlayerController());
            }
        }
    }
}

void ADAHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (UIManager)
    {
        UIManager->Teardown();
        UIManager = nullptr;
    }

    Super::EndPlay(EndPlayReason);
}

void ADAHUD::ToggleInventory()
{
    if (UIManager)
    {
        UIManager->ToggleInventory();
    }
}

void ADAHUD::ToggleQuestLog()
{
    if (UIManager)
    {
        UIManager->ToggleQuestLog();
    }
}

void ADAHUD::ShowNotification(const FText& Message, float Duration)
{
    if (UIManager)
    {
        UIManager->ShowNotification(Message, Duration);
    }
}

void ADAHUD::ShowInteractionPrompt(const FText& PromptText, const FText& ActionText)
{
    if (UIManager)
    {
        UIManager->ShowInteractionPrompt(PromptText, ActionText);
    }
}

void ADAHUD::HideInteractionPrompt()
{
    if (UIManager)
    {
        UIManager->HideInteractionPrompt();
    }
}

void ADAHUD::ShowDialogue(const FText& SpeakerName, const FText& DialogueText, const TArray<FText>& ResponseOptions)
{
    if (UIManager)
    {
        UIManager->ShowDialogue(SpeakerName, DialogueText, ResponseOptions);
    }
}

void ADAHUD::HideDialogue()
{
    if (UIManager)
    {
        UIManager->HideDialogue();
    }
}

void ADAHUD::UpdatePlayerStatus(float Health, float MaxHealth, float Stamina, float MaxStamina, float Hunger, float MaxHunger, float Thirst, float MaxThirst)
{
    if (UIManager)
    {
        UIManager->UpdatePlayerStatus(Health, MaxHealth, Stamina, MaxStamina, Hunger, MaxHunger, Thirst, MaxThirst);
    }
}