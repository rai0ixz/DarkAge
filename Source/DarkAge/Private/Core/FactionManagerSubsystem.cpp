#include "Core/FactionManagerSubsystem.h"
#include "Data/FactionData.h"
#include "Core/GameDebugManagerSubsystem.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Serialization/ArchiveSaveCompressedProxy.h"
#include "Serialization/ArchiveLoadCompressedProxy.h"
#include "Engine/Engine.h"
#include "UObject/UObjectGlobals.h"
#include "Misc/CompressionFlags.h"

void UFactionManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Check for a reset flag to force-delete corrupted save file
	FString ResetFilePath = FPaths::ProjectSavedDir() + TEXT("ResetFactions.flag");
	if (IFileManager::Get().FileExists(*ResetFilePath))
	{
		UE_LOG(LogTemp, Warning, TEXT("ResetFactions.flag found. Deleting Factions.sav."));
		FString SavePath = FPaths::ProjectSavedDir() + TEXT("Factions.sav");
		IFileManager::Get().Delete(*SavePath);
		IFileManager::Get().Delete(*ResetFilePath); // Delete the flag file itself
	}

	LoadFactions();
	if (Factions.Num() == 0)
	{
		CreateDefaultFactions();
	}
}

void UFactionManagerSubsystem::Deinitialize()
{
	SaveFactions();
	Super::Deinitialize();
}

void UFactionManagerSubsystem::CreateFaction(FName FactionID, const FText& DisplayName)
{
	if (!Factions.Contains(FactionID))
	{
		FFaction NewFaction;
		NewFaction.DisplayName = DisplayName;
		Factions.Add(FactionID, NewFaction);
		UE_LOG(LogTemp, Log, TEXT("Faction '%s' created."), *DisplayName.ToString());
	}
}

bool UFactionManagerSubsystem::GetFactionData(FName FactionID, FFaction& OutFaction) const
{
	if (const FFaction* FactionData = Factions.Find(FactionID))
	{
		OutFaction = *FactionData;
		return true;
	}
	return false;
}

FText UFactionManagerSubsystem::GetFactionDisplayName(FName FactionID) const
{
	if (const FFaction* FactionData = Factions.Find(FactionID))
	{
		return FactionData->DisplayName;
	}
	return FText::GetEmpty();
}

void UFactionManagerSubsystem::SetPlayerReputation(FName FactionID, float NewReputation)
{
	PlayerFactionReputations.FindOrAdd(FactionID) = NewReputation;
	OnPlayerReputationChanged.Broadcast(FactionID, NewReputation);
}

void UFactionManagerSubsystem::ModifyPlayerReputation(FName FactionID, float Delta)
{
	float& Reputation = PlayerFactionReputations.FindOrAdd(FactionID);
	Reputation += Delta;
	OnPlayerReputationChanged.Broadcast(FactionID, Reputation);
}

float UFactionManagerSubsystem::GetPlayerReputation(FName FactionID) const
{
	const float* Reputation = PlayerFactionReputations.Find(FactionID);
	return Reputation ? *Reputation : 0.0f;
}

EFactionRelationshipType UFactionManagerSubsystem::GetFactionRelationship(FName FactionA, FName FactionB) const
{
	if (const FFaction* FactionDataA = Factions.Find(FactionA))
	{
		if (const EFactionRelationshipType* Relationship = FactionDataA->Relationships.Find(FactionB))
		{
			return *Relationship;
		}
	}
	return EFactionRelationshipType::Neutral;
}

void UFactionManagerSubsystem::SetFactionRelationship(FName FactionA, FName FactionB, EFactionRelationshipType NewRelationship)
{
	if (FFaction* FactionDataA = Factions.Find(FactionA))
	{
		FactionDataA->Relationships.FindOrAdd(FactionB) = NewRelationship;
	}
	if (FFaction* FactionDataB = Factions.Find(FactionB))
	{
		FactionDataB->Relationships.FindOrAdd(FactionA) = NewRelationship;
	}
}

TArray<FName> UFactionManagerSubsystem::GetAllFactions() const
{
	TArray<FName> FactionIDs;
	Factions.GetKeys(FactionIDs);
	return FactionIDs;
}

bool UFactionManagerSubsystem::GetFactionInfo(EFactionID FactionID, FFactionInfoData& OutFactionInfo) const
{
	// This is a placeholder. A more robust implementation would map EFactionID to FName.
	const FName FactionName = GetFactionNameFromID(FactionID);
	if (const FFaction* Faction = Factions.Find(FactionName))
	{
		OutFactionInfo.FactionID = FactionName;
		OutFactionInfo.DisplayName = Faction->DisplayName;
		OutFactionInfo.Reputation = GetPlayerReputation(FactionName);
		return true;
	}
	return false;
}

void UFactionManagerSubsystem::ModifyFactionRelationship(FName Faction1ID, FName Faction2ID, int32 Delta, bool bMutual)
{
	auto Modify = [&](FName F1, FName F2)
	{
		if (FFaction* Faction = Factions.Find(F1))
		{
			EFactionRelationshipType& Relationship = Faction->Relationships.FindOrAdd(F2, EFactionRelationshipType::Neutral);
			int32 NewRelationship = FMath::Clamp(static_cast<int32>(Relationship) + Delta, 0, static_cast<int32>(EFactionRelationshipType::Allied));
			Relationship = static_cast<EFactionRelationshipType>(NewRelationship);
		}
	};

	Modify(Faction1ID, Faction2ID);
	if (bMutual)
	{
		Modify(Faction2ID, Faction1ID);
	}
}

void UFactionManagerSubsystem::SimulateFactionAI(float DeltaTime)
{
	// Placeholder for more complex faction AI simulation
}

FName UFactionManagerSubsystem::GetTerritoryController(const FName& RegionID) const
{
	if (const FName* FactionID = TerritoryControl.Find(RegionID))
	{
		return *FactionID;
	}
	return NAME_None;
}

void UFactionManagerSubsystem::SetTerritoryControl(const FName& RegionID, const FName& FactionID, float Influence)
{
	TerritoryControl.FindOrAdd(RegionID) = FactionID;
	// Influence could be used for more complex logic, like contested territories
}

void UFactionManagerSubsystem::SaveFactions() const
{
	TArray<uint8> SaveData;
	FString SavePath = FPaths::ProjectSavedDir() + TEXT("Factions.sav");

	{
		FArchiveSaveCompressedProxy Compressor(SaveData, NAME_Zlib);

		int32 SaveVersion = 1;
		Compressor << SaveVersion;

		TMap<FName, FFaction> NonConstFactions = Factions;
		TMap<FName, float> NonConstReputations = PlayerFactionReputations;

		Compressor << NonConstFactions;
		Compressor << NonConstReputations;
	}

	if (SaveData.Num() > 0)
	{
		if (FFileHelper::SaveArrayToFile(SaveData, *SavePath))
		{
			UE_LOG(LogTemp, Log, TEXT("Successfully saved faction data to %s"), *SavePath);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to save faction data to %s"), *SavePath);
		}
	}
}

void UFactionManagerSubsystem::LoadFactions()
{
	TArray<uint8> LoadData;
	FString SavePath = FPaths::ProjectSavedDir() + TEXT("Factions.sav");
	if (!FFileHelper::LoadFileToArray(LoadData, *SavePath) || LoadData.Num() == 0)
	{
		return;
	}

	FArchiveLoadCompressedProxy Decompressor(LoadData, NAME_Zlib);
	if (Decompressor.IsError())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to decompress Factions.sav. The file may be corrupt. Deleting and re-initializing."));
		IFileManager::Get().Delete(*SavePath);
		return;
	}

	const int32 CurrentVersion = 1;
	int32 SaveVersion = 0;
	Decompressor << SaveVersion;

	if (Decompressor.IsError() || SaveVersion != CurrentVersion)
	{
		UE_LOG(LogTemp, Warning, TEXT("Factions.sav is an old version or corrupt. Version found: %d, expected: %d. Deleting and re-initializing."), SaveVersion, CurrentVersion);
		IFileManager::Get().Delete(*SavePath);
		return;
	}

	Decompressor << Factions;
	Decompressor << PlayerFactionReputations;

	if (Decompressor.IsError())
	{
		UE_LOG(LogTemp, Error, TEXT("Error deserializing Factions.sav after version check. The file may be corrupt. Deleting and re-initializing."));
		Factions.Empty();
		PlayerFactionReputations.Empty();
		IFileManager::Get().Delete(*SavePath);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Successfully loaded faction data from %s"), *SavePath);
	}
}

void UFactionManagerSubsystem::CreateDefaultFactions()
{
	CreateFaction(FName(TEXT("TheIronBrotherhood")), FText::FromString(TEXT("The Iron Brotherhood")));
	CreateFaction(FName(TEXT("TheMerchantCoalition")), FText::FromString(TEXT("The Merchant Coalition")));
	CreateFaction(FName(TEXT("TheCircleOfDruids")), FText::FromString(TEXT("The Circle of Druids")));
	CreateFaction(FName(TEXT("TheScholarsOfSundering")), FText::FromString(TEXT("The Scholars of the Sundering")));

	SetFactionRelationship(FName(TEXT("TheIronBrotherhood")), FName(TEXT("TheMerchantCoalition")), EFactionRelationshipType::Neutral);
	SetFactionRelationship(FName(TEXT("TheIronBrotherhood")), FName(TEXT("TheCircleOfDruids")), EFactionRelationshipType::Hostile);
	SetFactionRelationship(FName(TEXT("TheMerchantCoalition")), FName(TEXT("TheCircleOfDruids")), EFactionRelationshipType::Neutral);
}


FName UFactionManagerSubsystem::GetFactionNameFromID(EFactionID FactionID) const
{
	// This is a placeholder mapping. A more robust system would use a DataTable or a config file.
	switch (FactionID)
	{
		case EFactionID::TheIronBrotherhood: return FName(TEXT("TheIronBrotherhood"));
		case EFactionID::TheMerchantCoalition: return FName(TEXT("TheMerchantCoalition"));
		case EFactionID::TheCircleOfDruids: return FName(TEXT("TheCircleOfDruids"));
		case EFactionID::TheScholarsOfSundering: return FName(TEXT("TheScholarsOfSundering"));
		default: return NAME_None;
	}
}

bool UFactionManagerSubsystem::GetFactionState(FName FactionID, FFactionData& OutFactionState) const
{
    if (const FFaction* FoundFaction = Factions.Find(FactionID))
    {
        // Convert FFaction to FFactionData
        OutFactionState.FactionName = FoundFaction->DisplayName.ToString();
        OutFactionState.FactionDescription = TEXT("Generated from faction data"); // Default description
        OutFactionState.PowerLevel = 50.0f; // Default power level
        OutFactionState.Influence = 50.0f; // Default influence
        OutFactionState.PublicReputation = 0.0f; // Default reputation
        OutFactionState.bIsHostileToPlayer = false; // Default hostility
        OutFactionState.bIsPublicFaction = true; // Default visibility
        
        // Convert relationships if needed
        for (const auto& Relationship : FoundFaction->Relationships)
        {
            if (Relationship.Value == EFactionRelationshipType::Allied)
            {
                OutFactionState.AlliedFactions.Add(Relationship.Key);
            }
            else if (Relationship.Value == EFactionRelationshipType::Hostile)
            {
                OutFactionState.EnemyFactions.Add(Relationship.Key);
            }
        }
        
        return true;
    }
    return false;
}

void UFactionManagerSubsystem::UpdateFactionActions()
{
    UE_LOG(LogTemp, Log, TEXT("UpdateFactionActions: Updating faction actions and relationships"));
    
    // Update faction AI and relationships
    for (auto& FactionPair : Factions)
    {
        const FName& FactionID = FactionPair.Key;
        FFaction& FactionData = FactionPair.Value;
        
        UE_LOG(LogTemp, Log, TEXT("UpdateFactionActions: Processing faction '%s'"), *FactionData.DisplayName.ToString());
        
        // Example: Update faction relationships based on player reputation
        float PlayerReputation = GetPlayerReputation(FactionID);
        
        // If player has high reputation with this faction, improve relationships with allied factions
        if (PlayerReputation > 50.0f)
        {
            for (auto& RelationshipPair : FactionData.Relationships)
            {
                if (RelationshipPair.Value == EFactionRelationshipType::Allied)
                {
                    // Slightly improve player reputation with allied factions
                    float AlliedReputation = GetPlayerReputation(RelationshipPair.Key);
                    if (AlliedReputation < 100.0f)
                    {
                        ModifyPlayerReputation(RelationshipPair.Key, 1.0f);
                        UE_LOG(LogTemp, Log, TEXT("UpdateFactionActions: Improved reputation with allied faction '%s' due to high reputation with '%s'"),
                            *RelationshipPair.Key.ToString(), *FactionID.ToString());
                    }
                }
            }
        }
        
        // If player has low reputation with this faction, worsen relationships with hostile factions
        if (PlayerReputation < -50.0f)
        {
            for (auto& RelationshipPair : FactionData.Relationships)
            {
                if (RelationshipPair.Value == EFactionRelationshipType::Hostile)
                {
                    // Slightly worsen player reputation with hostile factions
                    float HostileReputation = GetPlayerReputation(RelationshipPair.Key);
                    if (HostileReputation > -100.0f)
                    {
                        ModifyPlayerReputation(RelationshipPair.Key, -1.0f);
                        UE_LOG(LogTemp, Log, TEXT("UpdateFactionActions: Worsened reputation with hostile faction '%s' due to low reputation with '%s'"),
                            *RelationshipPair.Key.ToString(), *FactionID.ToString());
                    }
                }
            }
        }
    }
    
    // Update territory control based on faction power
    for (const auto& TerritoryPair : TerritoryControl)
    {
        const FName& RegionID = TerritoryPair.Key;
        const FName& ControllingFactionID = TerritoryPair.Value;
        
        UE_LOG(LogTemp, Log, TEXT("UpdateFactionActions: Region '%s' controlled by faction '%s'"),
            *RegionID.ToString(), *ControllingFactionID.ToString());
    }
    
    UE_LOG(LogTemp, Log, TEXT("UpdateFactionActions: Faction actions update completed"));
}

// Advanced Political System Implementation
bool UFactionManagerSubsystem::DeclareWar(FName Faction1ID, FName Faction2ID)
{
    if (!Factions.Contains(Faction1ID) || !Factions.Contains(Faction2ID))
    {
        UE_LOG(LogTemp, Error, TEXT("DeclareWar: One or both factions do not exist"));
        return false;
    }
    
    if (Faction1ID == Faction2ID)
    {
        UE_LOG(LogTemp, Error, TEXT("DeclareWar: Cannot declare war on self"));
        return false;
    }
    
    // Set hostile relationships
    SetFactionRelationship(Faction1ID, Faction2ID, EFactionRelationshipType::Hostile);
    
    // Create war declaration event
    FPoliticalEvent WarEvent;
    WarEvent.EventType = EPoliticalEventType::WarDeclaration;
    WarEvent.PrimaryFactionID = Faction1ID;
    WarEvent.SecondaryFactionID = Faction2ID;
    WarEvent.Description = FText::FromString(FString::Printf(TEXT("%s declares war on %s"),
        *GetFactionDisplayName(Faction1ID).ToString(),
        *GetFactionDisplayName(Faction2ID).ToString()));
    WarEvent.Intensity = 0.9f;
    
    PoliticalEvents.Add(WarEvent);
    
    // Apply economic consequences
    ApplyWarEconomicEffects(Faction1ID, Faction2ID);
    
    // Affect allied factions
    PropagateWarEffects(Faction1ID, Faction2ID);
    
    UE_LOG(LogTemp, Warning, TEXT("War declared between %s and %s"),
        *Faction1ID.ToString(), *Faction2ID.ToString());
    
    OnPoliticalEventOccurred.Broadcast(WarEvent);
    return true;
}

bool UFactionManagerSubsystem::FormAlliance(FName Faction1ID, FName Faction2ID)
{
    if (!Factions.Contains(Faction1ID) || !Factions.Contains(Faction2ID))
    {
        UE_LOG(LogTemp, Error, TEXT("FormAlliance: One or both factions do not exist"));
        return false;
    }
    
    if (Faction1ID == Faction2ID)
    {
        UE_LOG(LogTemp, Error, TEXT("FormAlliance: Cannot form alliance with self"));
        return false;
    }
    
    // Check if already allied
    if (GetFactionRelationship(Faction1ID, Faction2ID) == EFactionRelationshipType::Allied)
    {
        UE_LOG(LogTemp, Warning, TEXT("FormAlliance: Factions are already allied"));
        return false;
    }
    
    // Set allied relationships
    SetFactionRelationship(Faction1ID, Faction2ID, EFactionRelationshipType::Allied);
    
    // Create alliance formation event
    FPoliticalEvent AllianceEvent;
    AllianceEvent.EventType = EPoliticalEventType::AllianceFormed;
    AllianceEvent.PrimaryFactionID = Faction1ID;
    AllianceEvent.SecondaryFactionID = Faction2ID;
    AllianceEvent.Description = FText::FromString(FString::Printf(TEXT("%s forms alliance with %s"),
        *GetFactionDisplayName(Faction1ID).ToString(),
        *GetFactionDisplayName(Faction2ID).ToString()));
    AllianceEvent.Intensity = 0.6f;
    
    PoliticalEvents.Add(AllianceEvent);
    
    // Apply economic benefits
    ApplyAllianceEconomicEffects(Faction1ID, Faction2ID);
    
    // Improve player reputation with both factions if player has good standing
    float Player1Rep = GetPlayerReputation(Faction1ID);
    float Player2Rep = GetPlayerReputation(Faction2ID);
    
    if (Player1Rep > 50.0f)
    {
        ModifyPlayerReputation(Faction2ID, 10.0f);
    }
    if (Player2Rep > 50.0f)
    {
        ModifyPlayerReputation(Faction1ID, 10.0f);
    }
    
    UE_LOG(LogTemp, Log, TEXT("Alliance formed between %s and %s"),
        *Faction1ID.ToString(), *Faction2ID.ToString());
    
    OnPoliticalEventOccurred.Broadcast(AllianceEvent);
    return true;
}

bool UFactionManagerSubsystem::NegotiatePeace(FName Faction1ID, FName Faction2ID)
{
    if (!Factions.Contains(Faction1ID) || !Factions.Contains(Faction2ID))
    {
        UE_LOG(LogTemp, Error, TEXT("NegotiatePeace: One or both factions do not exist"));
        return false;
    }
    
    // Check if factions are actually at war
    if (GetFactionRelationship(Faction1ID, Faction2ID) != EFactionRelationshipType::Hostile)
    {
        UE_LOG(LogTemp, Warning, TEXT("NegotiatePeace: Factions are not at war"));
        return false;
    }
    
    // Calculate peace negotiation success chance
    float SuccessChance = CalculatePeaceNegotiationChance(Faction1ID, Faction2ID);
    
    if (FMath::FRand() > SuccessChance)
    {
        UE_LOG(LogTemp, Warning, TEXT("Peace negotiation failed between %s and %s"),
            *Faction1ID.ToString(), *Faction2ID.ToString());
        return false;
    }
    
    // Set neutral relationships
    SetFactionRelationship(Faction1ID, Faction2ID, EFactionRelationshipType::Neutral);
    
    // Create peace treaty event
    FPoliticalEvent PeaceEvent;
    PeaceEvent.EventType = EPoliticalEventType::PeaceTreaty;
    PeaceEvent.PrimaryFactionID = Faction1ID;
    PeaceEvent.SecondaryFactionID = Faction2ID;
    PeaceEvent.Description = FText::FromString(FString::Printf(TEXT("Peace negotiated between %s and %s"),
        *GetFactionDisplayName(Faction1ID).ToString(),
        *GetFactionDisplayName(Faction2ID).ToString()));
    PeaceEvent.Intensity = 0.7f;
    
    PoliticalEvents.Add(PeaceEvent);
    
    // Apply peace benefits
    ApplyPeaceEconomicEffects(Faction1ID, Faction2ID);
    
    UE_LOG(LogTemp, Log, TEXT("Peace negotiated between %s and %s"),
        *Faction1ID.ToString(), *Faction2ID.ToString());
    
    OnPoliticalEventOccurred.Broadcast(PeaceEvent);
    return true;
}

void UFactionManagerSubsystem::ApplyWarEconomicEffects(FName Faction1ID, FName Faction2ID)
{
    // War is expensive - reduce faction wealth
    if (FFaction* Faction1 = Factions.Find(Faction1ID))
    {
        // Reduce trade efficiency and wealth
        UE_LOG(LogTemp, Log, TEXT("War economic effects applied to %s"), *Faction1ID.ToString());
    }
    
    if (FFaction* Faction2 = Factions.Find(Faction2ID))
    {
        // Reduce trade efficiency and wealth
        UE_LOG(LogTemp, Log, TEXT("War economic effects applied to %s"), *Faction2ID.ToString());
    }
    
    // Disrupt trade routes between warring factions
    DisruptTradeBetweenFactions(Faction1ID, Faction2ID);
}

void UFactionManagerSubsystem::ApplyAllianceEconomicEffects(FName Faction1ID, FName Faction2ID)
{
    // Alliances improve trade
    UE_LOG(LogTemp, Log, TEXT("Alliance economic benefits applied between %s and %s"),
        *Faction1ID.ToString(), *Faction2ID.ToString());
    
    // Improve trade efficiency between allied factions
    ImproveTradeBetweenFactions(Faction1ID, Faction2ID);
}

void UFactionManagerSubsystem::ApplyPeaceEconomicEffects(FName Faction1ID, FName Faction2ID)
{
    // Peace allows trade to resume
    UE_LOG(LogTemp, Log, TEXT("Peace economic effects applied between %s and %s"),
        *Faction1ID.ToString(), *Faction2ID.ToString());
    
    // Restore normal trade relations
    RestoreTradeBetweenFactions(Faction1ID, Faction2ID);
}

void UFactionManagerSubsystem::PropagateWarEffects(FName Faction1ID, FName Faction2ID)
{
    // Allied factions may be drawn into the conflict
    if (FFaction* Faction1 = Factions.Find(Faction1ID))
    {
        for (const auto& RelationshipPair : Faction1->Relationships)
        {
            if (RelationshipPair.Value == EFactionRelationshipType::Allied)
            {
                FName AlliedFactionID = RelationshipPair.Key;
                
                // Chance for allied faction to join the war
                if (FMath::FRand() < 0.3f) // 30% chance
                {
                    SetFactionRelationship(AlliedFactionID, Faction2ID, EFactionRelationshipType::Hostile);
                    UE_LOG(LogTemp, Warning, TEXT("%s joins the war against %s due to alliance with %s"),
                        *AlliedFactionID.ToString(), *Faction2ID.ToString(), *Faction1ID.ToString());
                }
            }
        }
    }
}

float UFactionManagerSubsystem::CalculatePeaceNegotiationChance(FName Faction1ID, FName Faction2ID)
{
    float BaseChance = 0.5f; // 50% base chance
    
    // Player reputation affects peace chances
    float Player1Rep = GetPlayerReputation(Faction1ID);
    float Player2Rep = GetPlayerReputation(Faction2ID);
    
    // Higher player reputation with both factions improves peace chances
    if (Player1Rep > 50.0f && Player2Rep > 50.0f)
    {
        BaseChance += 0.3f;
    }
    else if (Player1Rep > 25.0f && Player2Rep > 25.0f)
    {
        BaseChance += 0.1f;
    }
    
    // Time since war started affects peace chances (longer wars are harder to end)
    // This would require tracking war start times in a real implementation
    
    return FMath::Clamp(BaseChance, 0.1f, 0.9f);
}

void UFactionManagerSubsystem::DisruptTradeBetweenFactions(FName Faction1ID, FName Faction2ID)
{
    UE_LOG(LogTemp, Warning, TEXT("Trade disrupted between warring factions %s and %s"),
        *Faction1ID.ToString(), *Faction2ID.ToString());
    // Implementation would integrate with economy system
}

void UFactionManagerSubsystem::ImproveTradeBetweenFactions(FName Faction1ID, FName Faction2ID)
{
    UE_LOG(LogTemp, Log, TEXT("Trade improved between allied factions %s and %s"),
        *Faction1ID.ToString(), *Faction2ID.ToString());
    // Implementation would integrate with economy system
}

void UFactionManagerSubsystem::RestoreTradeBetweenFactions(FName Faction1ID, FName Faction2ID)
{
    UE_LOG(LogTemp, Log, TEXT("Trade restored between %s and %s after peace"),
        *Faction1ID.ToString(), *Faction2ID.ToString());
    // Implementation would integrate with economy system
}

// Advanced Political Event System
void UFactionManagerSubsystem::ProcessPoliticalEvents()
{
    // Clean up old events (older than 1 hour game time)
    // Remove old events (simplified without timestamp for now)
    if (PoliticalEvents.Num() > 100)
    {
        PoliticalEvents.RemoveAt(0, PoliticalEvents.Num() - 100);
    }
    
    // Generate random political events
    if (FMath::FRand() < 0.05f) // 5% chance per update
    {
        GenerateRandomPoliticalEvent();
    }
    
    // Process ongoing political tensions
    UpdatePoliticalTensions();
}

void UFactionManagerSubsystem::GenerateRandomPoliticalEvent()
{
    TArray<FName> FactionIDs;
    Factions.GetKeys(FactionIDs);
    
    if (FactionIDs.Num() < 2)
        return;
    
    // Select random factions
    FName Faction1 = FactionIDs[FMath::RandRange(0, FactionIDs.Num() - 1)];
    FName Faction2;
    do {
        Faction2 = FactionIDs[FMath::RandRange(0, FactionIDs.Num() - 1)];
    } while (Faction2 == Faction1);
    
    // Generate random event type
    int32 EventType = FMath::RandRange(0, 4);
    
    switch (EventType)
    {
    case 0: // Trade Dispute
        GenerateTradeDisputeEvent(Faction1, Faction2);
        break;
    case 1: // Trade Dispute (changed from Border Skirmish as it's not in enum)
        GenerateTradeDisputeEvent(Faction1, Faction2);
        break;
    case 2: // Diplomatic Marriage
        GenerateDiplomaticMarriageEvent(Faction1, Faction2);
        break;
    case 3: // Resource Discovery
        GenerateResourceDiscoveryEvent(Faction1);
        break;
    case 4: // Leadership Change
        GenerateLeadershipChangeEvent(Faction1);
        break;
    }
}

void UFactionManagerSubsystem::GenerateTradeDisputeEvent(FName Faction1ID, FName Faction2ID)
{
    FPoliticalEvent Event;
    Event.EventType = EPoliticalEventType::TradeDispute;
    Event.PrimaryFactionID = Faction1ID;
    Event.SecondaryFactionID = Faction2ID;
    Event.Description = FText::FromString(FString::Printf(TEXT("Trade dispute erupts between %s and %s"),
        *GetFactionDisplayName(Faction1ID).ToString(),
        *GetFactionDisplayName(Faction2ID).ToString()));
    if (UWorld* World = GetWorld())
    {
        Event.Timestamp = World->GetTimeSeconds();
    }
    Event.Severity = 0.4f;
    
    PoliticalEvents.Add(Event);
    
    // Slightly worsen relations
    ModifyFactionRelationship(Faction1ID, Faction2ID, -1, true);
    
    UE_LOG(LogTemp, Log, TEXT("Political Event: %s"), *Event.Description.ToString());
    OnPoliticalEventOccurred.Broadcast(Event);
}

void UFactionManagerSubsystem::GenerateBorderSkirmishEvent(FName Faction1ID, FName Faction2ID)
{
    FPoliticalEvent Event;
    Event.EventType = EPoliticalEventType::TerritorialDispute; // Use existing enum value
    Event.PrimaryFactionID = Faction1ID;
    Event.SecondaryFactionID = Faction2ID;
    Event.Description = FText::FromString(FString::Printf(TEXT("Border skirmish between %s and %s forces"),
        *GetFactionDisplayName(Faction1ID).ToString(),
        *GetFactionDisplayName(Faction2ID).ToString()));
    if (UWorld* World = GetWorld())
    {
        Event.Timestamp = World->GetTimeSeconds();
    }
    Event.Severity = 0.6f;
    
    PoliticalEvents.Add(Event);
    
    // Significantly worsen relations
    ModifyFactionRelationship(Faction1ID, Faction2ID, -2, true);
    
    UE_LOG(LogTemp, Warning, TEXT("Political Event: %s"), *Event.Description.ToString());
    OnPoliticalEventOccurred.Broadcast(Event);
}

void UFactionManagerSubsystem::GenerateDiplomaticMarriageEvent(FName Faction1ID, FName Faction2ID)
{
    // Only if factions are not hostile
    if (GetFactionRelationship(Faction1ID, Faction2ID) == EFactionRelationshipType::Hostile)
        return;
    
    FPoliticalEvent Event;
    Event.EventType = EPoliticalEventType::DiplomaticMarriage;
    Event.PrimaryFactionID = Faction1ID;
    Event.SecondaryFactionID = Faction2ID;
    Event.Description = FText::FromString(FString::Printf(TEXT("Diplomatic marriage arranged between %s and %s nobility"),
        *GetFactionDisplayName(Faction1ID).ToString(),
        *GetFactionDisplayName(Faction2ID).ToString()));
    if (UWorld* World = GetWorld())
    {
        Event.Timestamp = World->GetTimeSeconds();
    }
    Event.Severity = 0.5f;
    
    PoliticalEvents.Add(Event);
    
    // Improve relations
    ModifyFactionRelationship(Faction1ID, Faction2ID, 2, true);
    
    UE_LOG(LogTemp, Log, TEXT("Political Event: %s"), *Event.Description.ToString());
    OnPoliticalEventOccurred.Broadcast(Event);
}

void UFactionManagerSubsystem::GenerateResourceDiscoveryEvent(FName FactionID)
{
    FPoliticalEvent Event;
    Event.EventType = EPoliticalEventType::ResourceDiscovery;
    Event.PrimaryFactionID = FactionID;
    Event.Description = FText::FromString(FString::Printf(TEXT("%s discovers valuable resources in their territory"),
        *GetFactionDisplayName(FactionID).ToString()));
    if (UWorld* World = GetWorld())
    {
        Event.Timestamp = World->GetTimeSeconds();
    }
    Event.Severity = 0.3f;
    
    PoliticalEvents.Add(Event);
    
    // Improve player reputation if player has good standing
    if (GetPlayerReputation(FactionID) > 25.0f)
    {
        ModifyPlayerReputation(FactionID, 5.0f);
    }
    
    UE_LOG(LogTemp, Log, TEXT("Political Event: %s"), *Event.Description.ToString());
    OnPoliticalEventOccurred.Broadcast(Event);
}

void UFactionManagerSubsystem::GenerateLeadershipChangeEvent(FName FactionID)
{
    FPoliticalEvent Event;
    Event.EventType = EPoliticalEventType::LeadershipChange;
    Event.PrimaryFactionID = FactionID;
    Event.Description = FText::FromString(FString::Printf(TEXT("Leadership change occurs in %s"),
        *GetFactionDisplayName(FactionID).ToString()));
    if (UWorld* World = GetWorld())
    {
        Event.Timestamp = World->GetTimeSeconds();
    }
    Event.Severity = 0.4f;
    
    PoliticalEvents.Add(Event);
    
    // Reset some relationships (new leader, new policies)
    if (FFaction* Faction = Factions.Find(FactionID))
    {
        for (auto& RelationshipPair : Faction->Relationships)
        {
            if (FMath::FRand() < 0.3f) // 30% chance to change each relationship
            {
                int32 Change = FMath::RandRange(-1, 1);
                if (Change != 0)
                {
                    ModifyFactionRelationship(FactionID, RelationshipPair.Key, Change, false);
                }
            }
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("Political Event: %s"), *Event.Description.ToString());
    OnPoliticalEventOccurred.Broadcast(Event);
}

void UFactionManagerSubsystem::UpdatePoliticalTensions()
{
    // Check for escalating tensions that might lead to war
    for (const auto& Faction1Pair : Factions)
    {
        const FName& Faction1ID = Faction1Pair.Key;
        const FFaction& Faction1 = Faction1Pair.Value;
        
        for (const auto& RelationshipPair : Faction1.Relationships)
        {
            const FName& Faction2ID = RelationshipPair.Key;
            const EFactionRelationshipType& Relationship = RelationshipPair.Value;
            
            if (Relationship == EFactionRelationshipType::Hostile)
            {
                // Check if war should escalate or de-escalate
                if (FMath::FRand() < 0.02f) // 2% chance per update
                {
                    if (FMath::FRand() < 0.7f) // 70% chance to escalate, 30% to de-escalate
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Tensions escalating between %s and %s"),
                            *Faction1ID.ToString(), *Faction2ID.ToString());
                        // Could trigger larger conflicts or involve more factions
                    }
                    else
                    {
                        UE_LOG(LogTemp, Log, TEXT("Tensions cooling between %s and %s"),
                            *Faction1ID.ToString(), *Faction2ID.ToString());
                        // Chance for automatic peace negotiation
                        if (FMath::FRand() < 0.3f)
                        {
                            NegotiatePeace(Faction1ID, Faction2ID);
                        }
                    }
                }
            }
        }
    }
}

TArray<FPoliticalEvent> UFactionManagerSubsystem::GetRecentPoliticalEvents(int32 MaxEvents) const
{
    TArray<FPoliticalEvent> RecentEvents = PoliticalEvents;
    
    // Sort by timestamp (most recent first)
    RecentEvents.Sort([](const FPoliticalEvent& A, const FPoliticalEvent& B)
    {
        return A.Timestamp > B.Timestamp;
    });
    
    // Return only the requested number of events
    if (MaxEvents > 0 && RecentEvents.Num() > MaxEvents)
    {
        RecentEvents.SetNum(MaxEvents);
    }
    
    return RecentEvents;
}

float UFactionManagerSubsystem::GetPoliticalStability() const
{
    if (Factions.Num() == 0)
        return 1.0f;
    
    float TotalStability = 0.0f;
    int32 RelationshipCount = 0;
    
    // Calculate stability based on faction relationships
    for (const auto& FactionPair : Factions)
    {
        const FFaction& Faction = FactionPair.Value;
        
        for (const auto& RelationshipPair : Faction.Relationships)
        {
            switch (RelationshipPair.Value)
            {
            case EFactionRelationshipType::Allied:
                TotalStability += 1.0f;
                break;
            case EFactionRelationshipType::Neutral:
                TotalStability += 0.5f;
                break;
            case EFactionRelationshipType::Hostile:
                TotalStability += 0.0f;
                break;
            }
            RelationshipCount++;
        }
    }
    
    if (RelationshipCount == 0)
        return 1.0f;
    
    return TotalStability / RelationshipCount;
}