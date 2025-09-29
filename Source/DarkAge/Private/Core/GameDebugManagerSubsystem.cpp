#include "Core/GameDebugManagerSubsystem.h"
#include "BaseClass/DAPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InventoryComponent.h"
#include "Components/StatlineComponent.h"
#include "Components/CraftingComponent.h"
#include "Components/PlayerSkillsComponent.h"
#include "Components/FactionReputationComponent.h"
#include "Components/NotorietyComponent.h"
#include "Core/FactionManagerSubsystem.h"
#include "AI/DAAIController.h"
#include "AI/DAAIBaseCharacter.h"
#include "Core/WorldEcosystemSubsystem.h"
#include "Data/WorldEcosystemData.h"
#include "Core/AdvancedMarketSimulationSubsystem.h"
#include "Core/AdvancedSurvivalSubsystem.h"
#include "Core/NPCEcosystemSubsystem.h"
#include "Core/SocialSimulationSubsystem.h"
#include "Core/WorldPopulationSubsystem.h"

void UGameDebugManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
}

ADAPlayerCharacter* UGameDebugManagerSubsystem::GetPlayerCharacter() const
{
    if (GetWorld())
    {
        return Cast<ADAPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    }
    return nullptr;
}

// --- Player Character Debug Commands ---

void UGameDebugManagerSubsystem::AddInventoryItem(FName ItemID, int32 Quantity)
{
    if (ADAPlayerCharacter* Player = GetPlayerCharacter())
    {
        if (UInventoryComponent* Inventory = Player->FindComponentByClass<UInventoryComponent>())
        {
            Inventory->AddInventoryItem(ItemID, Quantity);
        }
    }
}

void UGameDebugManagerSubsystem::PrintInventory()
{
    if (ADAPlayerCharacter* Player = GetPlayerCharacter())
    {
        if (UInventoryComponent* Inventory = Player->FindComponentByClass<UInventoryComponent>())
        {
            Inventory->PrintInventory();
        }
    }
}

void UGameDebugManagerSubsystem::UseItemInSlot(int32 SlotIndex)
{
    if (ADAPlayerCharacter* Player = GetPlayerCharacter())
    {
        if (UInventoryComponent* Inventory = Player->FindComponentByClass<UInventoryComponent>())
        {
            // Inventory->UseItemInSlot(SlotIndex);
        }
    }
}

void UGameDebugManagerSubsystem::PrintPlayerStats()
{
    if (ADAPlayerCharacter* Player = GetPlayerCharacter())
    {
        if (UStatlineComponent* Statline = Player->FindComponentByClass<UStatlineComponent>())
        {
            Statline->PrintStats();
        }
    }
}

void UGameDebugManagerSubsystem::CraftItem(FName RecipeID)
{
    if (ADAPlayerCharacter* Player = GetPlayerCharacter())
    {
        if (UCraftingComponent* Crafting = Player->FindComponentByClass<UCraftingComponent>())
        {
            Crafting->CraftItem(RecipeID);
        }
    }
}

// --- Skills Debug Commands ---

void UGameDebugManagerSubsystem::AddSkillXP(FName SkillID, float XPAmount)
{
    if (ADAPlayerCharacter* Player = GetPlayerCharacter())
    {
        if (UPlayerSkillsComponent* Skills = Player->FindComponentByClass<UPlayerSkillsComponent>())
        {
            Skills->AddXP(SkillID, XPAmount);
        }
    }
}

void UGameDebugManagerSubsystem::PrintSkillLevel(FName SkillID)
{
    if (ADAPlayerCharacter* Player = GetPlayerCharacter())
    {
        if (UPlayerSkillsComponent* Skills = Player->FindComponentByClass<UPlayerSkillsComponent>())
        {
            UE_LOG(LogTemp, Warning, TEXT("Skill %s Level: %d"), *SkillID.ToString(), Skills->GetSkillLevel(SkillID));
        }
    }
}

void UGameDebugManagerSubsystem::PrintAllSkills()
{
    if (ADAPlayerCharacter* Player = GetPlayerCharacter())
    {
        if (UPlayerSkillsComponent* Skills = Player->FindComponentByClass<UPlayerSkillsComponent>())
        {
            Skills->PrintAllSkills();
        }
    }
}

void UGameDebugManagerSubsystem::UnlockPerk(FName SkillID, FName PerkID)
{
    if (ADAPlayerCharacter* Player = GetPlayerCharacter())
    {
        if (UPlayerSkillsComponent* Skills = Player->FindComponentByClass<UPlayerSkillsComponent>())
        {
            Skills->UnlockPerk(SkillID, PerkID);
        }
    }
}

// --- Crime Debug Commands ---

void UGameDebugManagerSubsystem::SimulateCrime(FString CrimeTypeString, float NotorietyValue)
{
    if (ADAPlayerCharacter* Player = GetPlayerCharacter())
    {
        if (UNotorietyComponent* Notoriety = Player->FindComponentByClass<UNotorietyComponent>())
        {
            // Notoriety->RecordCrime(ECrimeType::Theft, NotorietyValue, Player, nullptr);
        }
    }
}

// --- Faction Debug Commands ---

void UGameDebugManagerSubsystem::PrintFactionReputation(FString FactionIDString)
{
    if (ADAPlayerCharacter* Player = GetPlayerCharacter())
    {
        if (UFactionReputationComponent* FactionReputation = Player->FindComponentByClass<UFactionReputationComponent>())
        {
            // FactionReputation->PrintReputation(FactionIDString);
        }
    }
}

void UGameDebugManagerSubsystem::ModifyFactionReputation(FString FactionIDString, float Delta)
{
    if (ADAPlayerCharacter* Player = GetPlayerCharacter())
    {
        if (UFactionReputationComponent* FactionReputation = Player->FindComponentByClass<UFactionReputationComponent>())
        {
            FName FactionName(*FactionIDString);
            FactionReputation->ModifyReputation(FactionName, Delta);
        }
    }
}

void UGameDebugManagerSubsystem::PrintFactionRelationships(FString FactionIDString)
{
    if (ADAPlayerCharacter* Player = GetPlayerCharacter())
    {
        if (UFactionReputationComponent* FactionReputation = Player->FindComponentByClass<UFactionReputationComponent>())
        {
            // FactionReputation->PrintRelationships(FactionIDString);
        }
    }
}

void UGameDebugManagerSubsystem::SimulateFactionAI()
{
    if (GetWorld())
    {
        if (UFactionManagerSubsystem* FactionManager = GetWorld()->GetGameInstance()->GetSubsystem<UFactionManagerSubsystem>())
        {
            FactionManager->UpdateFactionActions();
        }
    }
   }
   
   void UGameDebugManagerSubsystem::SpawnAIPopulation(int32 Amount, const FString& AIBlueprintPath)
   {
       if (ADAPlayerCharacter* Player = GetPlayerCharacter())
       {
           UWorld* World = Player->GetWorld();
           if (!World)
           {
               return;
           }
   
           UClass* AIBlueprintClass = LoadObject<UClass>(nullptr, *AIBlueprintPath);
           if (!AIBlueprintClass)
           {
               UE_LOG(LogTemp, Warning, TEXT("DebugManager: Failed to load AI Blueprint: %s"), *AIBlueprintPath);
               return;
           }
   
           const FVector SpawnLocation = Player->GetActorLocation() + (Player->GetActorForwardVector() * 200.0f);
           const FRotator SpawnRotation = Player->GetActorRotation();
           FActorSpawnParameters SpawnParams;
           SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
   
           for (int32 i = 0; i < Amount; ++i)
           {
               // Slightly randomize position to avoid stacking
               FVector RandomizedLocation = SpawnLocation + FVector(FMath::RandRange(-500.f, 500.f), FMath::RandRange(-500.f, 500.f), 0.f);
               World->SpawnActor<AActor>(AIBlueprintClass, RandomizedLocation, SpawnRotation, SpawnParams);
           }
   
           UE_LOG(LogTemp, Warning, TEXT("DebugManager: Spawned %d AI from %s"), Amount, *AIBlueprintPath);
       }
   }
   
   // --- Political System Debug Commands ---
   
   void UGameDebugManagerSubsystem::PrintActiveTreaties(FString FactionIDString)
{
    if (ADAPlayerCharacter* Player = GetPlayerCharacter())
    {
        // Player->PrintActiveTreatiesDebug(FactionIDString);
    }
}

void UGameDebugManagerSubsystem::CreateTreaty(FString TreatyTypeString, FString Faction1String, FString Faction2String, int32 DurationDays)
{
    if (ADAPlayerCharacter* Player = GetPlayerCharacter())
    {
        // Player->CreateTreatyDebug(TreatyTypeString, Faction1String, Faction2String, DurationDays);
    }
}

void UGameDebugManagerSubsystem::TerminateTreaty(FString TreatyIDString)
{
    if (ADAPlayerCharacter* Player = GetPlayerCharacter())
    {
        // Player->TerminateTreatyDebug(TreatyIDString);
    }
}

void UGameDebugManagerSubsystem::PrintActivePoliticalEvents(FString RegionIDString)
{
    if (ADAPlayerCharacter* Player = GetPlayerCharacter())
    {
        // Player->PrintActivePoliticalEventsDebug(RegionIDString);
    }
}

void UGameDebugManagerSubsystem::CreatePoliticalEvent(FString EventTypeString, FString PrimaryFactionString, FString SecondaryFactionString, FString RegionIDString, float Intensity)
{
    if (ADAPlayerCharacter* Player = GetPlayerCharacter())
    {
        // Player->CreatePoliticalEventDebug(EventTypeString, PrimaryFactionString, SecondaryFactionString, RegionIDString, Intensity);
    }
}

void UGameDebugManagerSubsystem::ResolvePoliticalEvent(FString EventIDString, bool bSuccessful)
{
    if (ADAPlayerCharacter* Player = GetPlayerCharacter())
    {
        // Player->ResolvePoliticalEventDebug(EventIDString, bSuccessful);
    }
}

void UGameDebugManagerSubsystem::SimulateWar(FString AttackerFactionString, FString DefenderFactionString)
{
    if (ADAPlayerCharacter* Player = GetPlayerCharacter())
    {
        // Player->SimulateWarDebug(AttackerFactionString, DefenderFactionString);
    }
}

void UGameDebugManagerSubsystem::NegotiatePeace(FString Faction1String, FString Faction2String)
{
    if (ADAPlayerCharacter* Player = GetPlayerCharacter())
    {
        // Player->NegotiatePeaceDebug(Faction1String, Faction2String);
    }
}

// --- Ecosystem Debug Commands ---

void UGameDebugManagerSubsystem::ToggleEcosystem(bool bIsEnabled)
{
    if (UWorldEcosystemSubsystem* EcosystemSubsystem = GetGameInstance()->GetSubsystem<UWorldEcosystemSubsystem>())
    {
        EcosystemSubsystem->AdminSetEcosystemEnabled(bIsEnabled);
    }
}

void UGameDebugManagerSubsystem::SetSeason(const FString& RegionName, const FString& SeasonName)
{
    if (UWorldEcosystemSubsystem* EcosystemSubsystem = GetGameInstance()->GetSubsystem<UWorldEcosystemSubsystem>())
    {
        ESeasonType Season = ESeasonType::Spring; // Default
        if (SeasonName.Equals("Summer", ESearchCase::IgnoreCase)) Season = ESeasonType::Summer;
        else if (SeasonName.Equals("Autumn", ESearchCase::IgnoreCase)) Season = ESeasonType::Autumn;
        else if (SeasonName.Equals("Winter", ESearchCase::IgnoreCase)) Season = ESeasonType::Winter;
        
        EcosystemSubsystem->AdminForceSeason(RegionName, Season);
    }
}

void UGameDebugManagerSubsystem::SetWeather(const FString& RegionName, const FString& WeatherName, float Intensity)
{
    if (UWorldEcosystemSubsystem* EcosystemSubsystem = GetGameInstance()->GetSubsystem<UWorldEcosystemSubsystem>())
    {
        EWeatherPattern Weather = EWeatherPattern::Clear; // Default
        if (WeatherName.Equals("Rain", ESearchCase::IgnoreCase)) Weather = EWeatherPattern::Rain;
        else if (WeatherName.Equals("Storm", ESearchCase::IgnoreCase)) Weather = EWeatherPattern::Storm;
        else if (WeatherName.Equals("Snow", ESearchCase::IgnoreCase)) Weather = EWeatherPattern::Snow;
        else if (WeatherName.Equals("Cloudy", ESearchCase::IgnoreCase)) Weather = EWeatherPattern::Cloudy;

        EcosystemSubsystem->AdminForceWeather(RegionName, Weather, Intensity);
    }
}

void UGameDebugManagerSubsystem::SetResource(const FString& RegionName, const FString& ResourceName, float Value)
{
    if (UWorldEcosystemSubsystem* EcosystemSubsystem = GetGameInstance()->GetSubsystem<UWorldEcosystemSubsystem>())
    {
        EBasicResourceType Resource = EBasicResourceType::Water; // Default
        if (ResourceName.Equals("Food", ESearchCase::IgnoreCase)) Resource = EBasicResourceType::Food;
        else if (ResourceName.Equals("Wood", ESearchCase::IgnoreCase)) Resource = EBasicResourceType::Wood;
        else if (ResourceName.Equals("Stone", ESearchCase::IgnoreCase)) Resource = EBasicResourceType::Stone;
        else if (ResourceName.Equals("Iron", ESearchCase::IgnoreCase)) Resource = EBasicResourceType::Iron;
        else if (ResourceName.Equals("Gold", ESearchCase::IgnoreCase)) Resource = EBasicResourceType::Gold;
        
        EcosystemSubsystem->AdminSetResource(RegionName, Resource, Value);
    }
}

void UGameDebugManagerSubsystem::SetAnimalPopulation(const FString& RegionName, const FString& SpeciesName, int32 NewPopulation)
{
    if (UWorldEcosystemSubsystem* EcosystemSubsystem = GetGameInstance()->GetSubsystem<UWorldEcosystemSubsystem>())
    {
        EcosystemSubsystem->AdminSetAnimalPopulation(RegionName, SpeciesName, NewPopulation);
    }
}

void UGameDebugManagerSubsystem::TriggerEvent(const FString& RegionName, const FString& EventName, float Severity, float Duration)
{
    if (UWorldEcosystemSubsystem* EcosystemSubsystem = GetGameInstance()->GetSubsystem<UWorldEcosystemSubsystem>())
    {
        EEnvironmentalEventType Event = EEnvironmentalEventType::Drought; // Default
        if (EventName.Equals("Flood", ESearchCase::IgnoreCase)) Event = EEnvironmentalEventType::Flood;
        else if (EventName.Equals("Wildfire", ESearchCase::IgnoreCase)) Event = EEnvironmentalEventType::Wildfire;
        else if (EventName.Equals("Disease", ESearchCase::IgnoreCase)) Event = EEnvironmentalEventType::Blight;
        else if (EventName.Equals("Migration", ESearchCase::IgnoreCase)) Event = EEnvironmentalEventType::AnimalMigration;
        
        EcosystemSubsystem->AdminTriggerEvent(RegionName, Event, Severity, Duration);
    }
}

void UGameDebugManagerSubsystem::ClearEvent(const FString& RegionName, const FString& EventName)
{
    if (UWorldEcosystemSubsystem* EcosystemSubsystem = GetGameInstance()->GetSubsystem<UWorldEcosystemSubsystem>())
    {
        EEnvironmentalEventType Event = EEnvironmentalEventType::Drought; // Default
        if (EventName.Equals("Flood", ESearchCase::IgnoreCase)) Event = EEnvironmentalEventType::Flood;
        else if (EventName.Equals("Wildfire", ESearchCase::IgnoreCase)) Event = EEnvironmentalEventType::Wildfire;
        else if (EventName.Equals("Disease", ESearchCase::IgnoreCase)) Event = EEnvironmentalEventType::Blight;
        else if (EventName.Equals("Migration", ESearchCase::IgnoreCase)) Event = EEnvironmentalEventType::AnimalMigration;
        
        EcosystemSubsystem->AdminClearEvent(RegionName, Event);
    }
}

void UGameDebugManagerSubsystem::ToggleMarket(bool bIsEnabled)
{
    if (UAdvancedMarketSimulationSubsystem* MarketSubsystem = GetGameInstance()->GetSubsystem<UAdvancedMarketSimulationSubsystem>())
    {
        MarketSubsystem->bMarketEnabled = bIsEnabled;
    }
}

void UGameDebugManagerSubsystem::ToggleSurvival(bool bIsEnabled)
{
    if (UAdvancedSurvivalSubsystem* SurvivalSubsystem = GetGameInstance()->GetSubsystem<UAdvancedSurvivalSubsystem>())
    {
        // SurvivalSubsystem->bSurvivalEnabled = bIsEnabled;
    }
}

void UGameDebugManagerSubsystem::ToggleNPCEcosystem(bool bIsEnabled)
{
    if (UNPCEcosystemSubsystem* NPCEcosystem = GetGameInstance()->GetSubsystem<UNPCEcosystemSubsystem>())
    {
        // NPCEcosystem->bNPCEcosystemEnabled = bIsEnabled;
    }
}

void UGameDebugManagerSubsystem::ToggleSocialSimulation(bool bIsEnabled)
{
    if (USocialSimulationSubsystem* SocialSimulation = GetGameInstance()->GetSubsystem<USocialSimulationSubsystem>())
    {
        // SocialSimulation->bSocialSimulationEnabled = bIsEnabled;
    }
}

void UGameDebugManagerSubsystem::ToggleWorldPopulation(bool bIsEnabled)
{
    if (UWorldPopulationSubsystem* WorldPopulation = GetGameInstance()->GetSubsystem<UWorldPopulationSubsystem>())
    {
        // WorldPopulation->bPopulationEnabled = bIsEnabled;
    }
}
