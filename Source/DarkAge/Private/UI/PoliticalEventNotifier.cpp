#include "UI/PoliticalEventNotifier.h"
#include "Core/PoliticalSystem.h"
#include "Core/FactionManagerSubsystem.h"
#include "Core/PlayerLegacySubsystem.h"
#include "Data/SocialData.h"
#include "Components/FactionReputationComponent.h"
#include "BaseClass/DAPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

// Initialize static instance
UPoliticalEventNotifier* UPoliticalEventNotifier::Instance = nullptr;

UPoliticalEventNotifier::UPoliticalEventNotifier()
    : PoliticalSystem(nullptr)
    , FactionManager(nullptr)
{
}

void UPoliticalEventNotifier::Initialize()
{
    // Get the world
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("PoliticalEventNotifier: Failed to get world."));
        return;
    }
    
    // Get the game instance
    UGameInstance* GameInstance = World->GetGameInstance();
    if (!GameInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("PoliticalEventNotifier: Failed to get game instance."));
        return;
    }
    
    // Get the political system
    PoliticalSystem = GameInstance->GetSubsystem<UPoliticalSystem>();
    if (!PoliticalSystem)
    {
        UE_LOG(LogTemp, Error, TEXT("PoliticalEventNotifier: Failed to get political system."));
        return;
    }
    
    // Get the faction manager
    FactionManager = GameInstance->GetSubsystem<UFactionManagerSubsystem>();
    if (!FactionManager)
    {
        UE_LOG(LogTemp, Error, TEXT("PoliticalEventNotifier: Failed to get faction manager."));
        return;
    }
    
    // Bind to political events
    if (PoliticalSystem)
    {
        PoliticalSystem->OnPoliticalEventOccurred.AddDynamic(this, &UPoliticalEventNotifier::HandlePoliticalEventOccurred);
        PoliticalSystem->OnTreatyChanged.AddDynamic(this, &UPoliticalEventNotifier::HandleTreatyChanged);
    }
    
    // Bind to faction relationship changes
    // FactionManager->OnFactionRelationshipChanged.AddDynamic(this, &UPoliticalEventNotifier::HandleFactionRelationshipChanged);
    
    UE_LOG(LogTemp, Display, TEXT("PoliticalEventNotifier: Initialized."));
}

void UPoliticalEventNotifier::Shutdown()
{
    // Unbind from political events
    if (PoliticalSystem)
    {
        PoliticalSystem->OnPoliticalEventOccurred.RemoveDynamic(this, &UPoliticalEventNotifier::HandlePoliticalEventOccurred);
        PoliticalSystem->OnTreatyChanged.RemoveDynamic(this, &UPoliticalEventNotifier::HandleTreatyChanged);
    }
    
    // Unbind from faction relationship changes
    if (FactionManager)
    {
        // FactionManager->OnFactionRelationshipChanged.RemoveDynamic(this, &UPoliticalEventNotifier::HandleFactionRelationshipChanged);
    }
    
    // Clear references
    PoliticalSystem = nullptr;
    FactionManager = nullptr;
    
    UE_LOG(LogTemp, Display, TEXT("PoliticalEventNotifier: Shutdown."));
}

UPoliticalEventNotifier* UPoliticalEventNotifier::GetInstance(UObject* WorldContextObject)
{
    if (!Instance)
    {
        UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
        if (!World)
        {
            return nullptr;
        }
        
        Instance = NewObject<UPoliticalEventNotifier>(World);
        Instance->Initialize();
        
        // Make sure the instance doesn't get garbage collected
        Instance->AddToRoot();
    }
    
    return Instance;
}

void UPoliticalEventNotifier::HandlePoliticalEventOccurred(const FPoliticalEvent& Event)
{
    // Check if the event is relevant to the player
    if (!IsEventRelevantToPlayer(Event))
    {
        return;
    }
    
    // Generate notification text (assume new event by default)
    FString NotificationText = GenerateEventNotificationText(Event, true);
    
    // Broadcast notification
    OnPoliticalNotification.Broadcast(NotificationText);
}

void UPoliticalEventNotifier::HandleTreatyChanged(const FPoliticalTreaty& Treaty)
{
    // Check if the treaty is relevant to the player
    if (!IsTreatyRelevantToPlayer(Treaty))
    {
        return;
    }
    
    // Generate notification text (assume new treaty by default)
    FString NotificationText = GenerateTreatyNotificationText(Treaty, true);
    
    // Broadcast notification
    OnPoliticalNotification.Broadcast(NotificationText);
}

void UPoliticalEventNotifier::HandleFactionRelationshipChanged(FName FactionID, FName OtherFactionID, EFactionRelationshipType NewRelationship)
{
    // Check if the relationship change is relevant to the player
    if (!IsRelationshipRelevantToPlayer(FactionID, OtherFactionID))
    {
        return;
    }
    
    // Generate notification text
    FString NotificationText = GenerateRelationshipNotificationText(FactionID, OtherFactionID, NewRelationship);
    
    // Broadcast notification
    OnPoliticalNotification.Broadcast(NotificationText);
}

FString UPoliticalEventNotifier::GenerateEventNotificationText(const FPoliticalEvent& Event, bool bIsNewEvent)
{
    FString NotificationText;
    
    // Get faction names
    FString PrimaryFactionName = GetFactionDisplayName(Event.PrimaryFactionID);
    FString SecondaryFactionName = GetFactionDisplayName(Event.SecondaryFactionID);
    FString RegionName = GetRegionDisplayName(Event.RegionID);
    
    // Generate text based on event type and whether it's new or resolved
    if (bIsNewEvent)
    {
        switch (Event.EventType)
        {
            case EPoliticalEventType::Assassination:
                NotificationText = FString::Printf(TEXT("Assassination: An important figure in %s has been assassinated."),
                    *PrimaryFactionName);
                break;
            case EPoliticalEventType::Scandal:
                NotificationText = FString::Printf(TEXT("Political Scandal: A scandal has erupted within %s."),
                    *PrimaryFactionName);
                break;
            case EPoliticalEventType::TerritorialDispute:
                NotificationText = FString::Printf(TEXT("Territorial Dispute: %s and %s are disputing territory in %s."),
                    *PrimaryFactionName, *SecondaryFactionName, *RegionName);
                break;
            case EPoliticalEventType::TradeOffer:
                NotificationText = FString::Printf(TEXT("Trade Offer: %s has made a trade offer to %s."),
                    *PrimaryFactionName, *SecondaryFactionName);
                break;
            default:
                NotificationText = FString::Printf(TEXT("Political Event: An event involving %s is unfolding."),
                    *PrimaryFactionName);
                break;
        }
    }
    else
    {
        // Event is being resolved
        NotificationText = FString::Printf(TEXT("Event Update: %s"), *Event.Description.ToString());
    }
    
    return NotificationText;
}

FString UPoliticalEventNotifier::GenerateTreatyNotificationText(const FPoliticalTreaty& Treaty, bool bIsNewTreaty)
{
    FString NotificationText;
    
    // Get faction names
    TArray<FString> FactionNames;
    for (const FName& FactionID : Treaty.InvolvedFactions)
    {
        FactionNames.Add(GetFactionDisplayName(FactionID));
    }
    
    // Generate text based on treaty type and whether it's new or terminated
    if (bIsNewTreaty)
    {
        switch (Treaty.TreatyType)
        {
            case ETreatyType::Alliance:
                NotificationText = FString::Printf(TEXT("Military Alliance: %s and %s have formed a military alliance."),
                    *FactionNames[0], *FactionNames[1]);
                break;
            case ETreatyType::Defensive:
                NotificationText = FString::Printf(TEXT("Defensive Pact: %s and %s have signed a defensive pact."),
                    *FactionNames[0], *FactionNames[1]);
                break;
            case ETreatyType::Trade:
                NotificationText = FString::Printf(TEXT("Trade Agreement: %s and %s have established a trade agreement."),
                    *FactionNames[0], *FactionNames[1]);
                break;
            case ETreatyType::NonAggression:
                NotificationText = FString::Printf(TEXT("Non-Aggression Pact: %s and %s have agreed not to attack each other."),
                    *FactionNames[0], *FactionNames[1]);
                break;
                
            default:
                NotificationText = FString::Printf(TEXT("Treaty: %s and %s have signed a treaty."), 
                    *FactionNames[0], *FactionNames[1]);
                break;
        }
    }
    else
    {
        // Treaty is being terminated
        switch (Treaty.TreatyType)
        {
            case ETreatyType::Alliance:
                NotificationText = FString::Printf(TEXT("Alliance Dissolved: The military alliance between %s and %s has ended."),
                    *FactionNames[0], *FactionNames[1]);
                break;
            case ETreatyType::Defensive:
                NotificationText = FString::Printf(TEXT("Pact Terminated: The defensive pact between %s and %s is no more."),
                    *FactionNames[0], *FactionNames[1]);
                break;
            case ETreatyType::Trade:
                NotificationText = FString::Printf(TEXT("Trade Halted: The trade agreement between %s and %s has been canceled."),
                    *FactionNames[0], *FactionNames[1]);
                break;
                
            default:
                NotificationText = FString::Printf(TEXT("Treaty Ended: The treaty between %s and %s has been terminated."), 
                    *FactionNames[0], *FactionNames[1]);
                break;
        }
    }
    
    return NotificationText;
}

FString UPoliticalEventNotifier::GenerateRelationshipNotificationText(FName FactionID, FName OtherFactionID, EFactionRelationshipType NewRelationship)
{
    FString NotificationText;
    
    // Get faction names
    FString FactionName = GetFactionDisplayName(FactionID);
    FString OtherFactionName = GetFactionDisplayName(OtherFactionID);
    
    // Generate text based on new relationship
    switch (NewRelationship)
    {
        case EFactionRelationshipType::Allied:
            NotificationText = FString::Printf(TEXT("Alliance: %s and %s are now allies."),
                *FactionName, *OtherFactionName);
            break;
            
        case EFactionRelationshipType::Neutral:
            NotificationText = FString::Printf(TEXT("Neutral Relations: %s and %s are now neutral toward each other."),
                *FactionName, *OtherFactionName);
            break;
            
        case EFactionRelationshipType::Hostile:
            NotificationText = FString::Printf(TEXT("Hostility: %s and %s are now hostile toward each other."),
                *FactionName, *OtherFactionName);
            break;
            
        default:
            NotificationText = FString::Printf(TEXT("Relationship Change: The relationship between %s and %s has changed."),
                *FactionName, *OtherFactionName);
            break;
    }
    
    return NotificationText;
}

bool UPoliticalEventNotifier::IsEventRelevantToPlayer(const FPoliticalEvent& Event)
{
    // Get the player character
    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }
    ADAPlayerCharacter* PlayerCharacter = Cast<ADAPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0));
    if (!PlayerCharacter)
    {
        return false;
    }
    
    // Get the faction reputation component
    UFactionReputationComponent* FactionReputationComponent = PlayerCharacter->FindComponentByClass<UFactionReputationComponent>();
    if (!FactionReputationComponent)
    {
        return false;
    }
    
    // Check if player has reputation with any involved faction
    bool bHasReputationWithPrimary = FactionReputationComponent->GetFactionReputation(Event.PrimaryFactionID) != 0.0f;
    bool bHasReputationWithSecondary = Event.SecondaryFactionID.IsNone() ? false : 
        FactionReputationComponent->GetFactionReputation(Event.SecondaryFactionID) != 0.0f;
    
    // Check if event is in a region the player has visited
    // This would require tracking visited regions, which isn't implemented yet
    // For now, assume all region events are relevant
    bool bIsInRelevantRegion = false;
    if (PlayerCharacter)
    {
        if (UPlayerLegacySubsystem* LegacySubsystem = World->GetGameInstance()->GetSubsystem<UPlayerLegacySubsystem>())
        {
            bIsInRelevantRegion = LegacySubsystem->HasVisitedRegion(Event.RegionID);
        }
    }
    
    // Event is relevant if player has reputation with any involved faction or it's in a relevant region
    return bHasReputationWithPrimary || bHasReputationWithSecondary || bIsInRelevantRegion;
}

bool UPoliticalEventNotifier::IsTreatyRelevantToPlayer(const FPoliticalTreaty& Treaty)
{
    // Get the player character
    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }
    ADAPlayerCharacter* PlayerCharacter = Cast<ADAPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0));
    if (!PlayerCharacter)
    {
        return false;
    }
    
    // Get the faction reputation component
    UFactionReputationComponent* FactionReputationComponent = PlayerCharacter->FindComponentByClass<UFactionReputationComponent>();
    if (!FactionReputationComponent)
    {
        return false;
    }
    
    // Check if player has reputation with any involved faction
    for (const FName& FactionID : Treaty.InvolvedFactions)
    {
        if (FactionReputationComponent->GetFactionReputation(FactionID) != 0.0f)
        {
            return true;
        }
    }
    
    // No relevant factions found
    return false;
}

bool UPoliticalEventNotifier::IsRelationshipRelevantToPlayer(FName FactionID, FName OtherFactionID)
{
    // Get the player character
    UWorld* World = GetWorld();
    if (!World)
    {
        return false;
    }
    ADAPlayerCharacter* PlayerCharacter = Cast<ADAPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(World, 0));
    if (!PlayerCharacter)
    {
        return false;
    }
    
    // Get the faction reputation component
    UFactionReputationComponent* FactionReputationComponent = PlayerCharacter->FindComponentByClass<UFactionReputationComponent>();
    if (!FactionReputationComponent)
    {
        return false;
    }
    
    // Check if player has reputation with either faction
    bool bHasReputationWithFirst = FactionReputationComponent->GetFactionReputation(FactionID) != 0.0f;
    bool bHasReputationWithSecond = FactionReputationComponent->GetFactionReputation(OtherFactionID) != 0.0f;
    
    // Relationship is relevant if player has reputation with either faction
    return bHasReputationWithFirst || bHasReputationWithSecond;
}

FString UPoliticalEventNotifier::GetFactionDisplayName(FName FactionID)
{
    if (FactionID.IsNone())
    {
        return TEXT("Unknown Faction");
    }
    
    if (FactionManager)
    {
        FFactionInfoData FactionDef;
        const UEnum* FactionEnum = StaticEnum<EFactionID>();
        if (FactionEnum)
        {
            const EFactionID FactionIDEnum = (EFactionID)FactionEnum->GetValueByName(FactionID);
            if (FactionManager->GetFactionInfo(FactionIDEnum, FactionDef))
            {
                return FactionDef.DisplayName.ToString();
            }
        }
    }
    
    return FactionID.ToString();
}

FString UPoliticalEventNotifier::GetRegionDisplayName(FName RegionID)
{
    if (RegionID.IsNone())
    {
        return TEXT("Unknown Region");
    }
    
    // For now, just return the region ID
    // In a full implementation, you would look up the region name from a data table
    return RegionID.ToString();
}