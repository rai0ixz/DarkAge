#include "Core/NPCEcosystemSubsystem.h"
#include "Core/FactionManagerSubsystem.h"
#include "Core/EconomySubsystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

UNPCEcosystemSubsystem::UNPCEcosystemSubsystem()
{
    // Initialize ecosystem parameters
    PopulationGrowthRate = 0.001f; // Very slow growth
    MaxPopulationPerSettlement = 500;
    MigrationThreshold = 0.3f;
    SocialInteractionRadius = 1000.0f;
    
    // Initialize timers
    EcosystemUpdateTimer = 0.0f;
    SocialUpdateTimer = 0.0f;
    MigrationUpdateTimer = 0.0f;
    
    UE_LOG(LogTemp, Log, TEXT("NPCEcosystemSubsystem constructor completed"));
}

void UNPCEcosystemSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    InitializeSettlements();
    InitializeNPCArchetypes();
    InitializeSocialNetworks();
    
    UE_LOG(LogTemp, Log, TEXT("NPCEcosystemSubsystem initialized with %d settlements"), Settlements.Num());
}

void UNPCEcosystemSubsystem::Deinitialize()
{
    // Clean up NPC data
    NPCPopulation.Empty();
    Settlements.Empty();
    SocialNetworks.Empty();
    
    Super::Deinitialize();
    UE_LOG(LogTemp, Log, TEXT("NPCEcosystemSubsystem deinitialized"));
}

void UNPCEcosystemSubsystem::Tick(float DeltaTime)
{
    if (!bNPCEcosystemEnabled)
    {
        return;
    }

    EcosystemUpdateTimer += DeltaTime;
    SocialUpdateTimer += DeltaTime;
    MigrationUpdateTimer += DeltaTime;
    
    // Update ecosystem every 30 seconds
    if (EcosystemUpdateTimer >= 30.0f)
    {
        UpdateEcosystem();
        EcosystemUpdateTimer = 0.0f;
    }
    
    // Update social interactions every 10 seconds
    if (SocialUpdateTimer >= 10.0f)
    {
        UpdateSocialInteractions();
        SocialUpdateTimer = 0.0f;
    }
    
    // Update migration patterns every 60 seconds
    if (MigrationUpdateTimer >= 60.0f)
    {
        UpdateMigrationPatterns();
        MigrationUpdateTimer = 0.0f;
    }
    
    // Update individual NPC behaviors
    UpdateNPCBehaviors(DeltaTime);
}

void UNPCEcosystemSubsystem::InitializeSettlements()
{
    // Create major settlements
    CreateSettlement(TEXT("Millhaven"), FVector(0, 0, 0), ESettlementType::Village, 150);
    CreateSettlement(TEXT("Oakstead"), FVector(5000, 0, 0), ESettlementType::Town, 300);
    CreateSettlement(TEXT("Ironhold"), FVector(-3000, 4000, 0), ESettlementType::City, 450);
    CreateSettlement(TEXT("Riverside"), FVector(2000, -3000, 0), ESettlementType::Village, 120);
    CreateSettlement(TEXT("Goldport"), FVector(8000, 6000, 0), ESettlementType::Port, 280);
    
    UE_LOG(LogTemp, Log, TEXT("Initialized %d settlements"), Settlements.Num());
}

void UNPCEcosystemSubsystem::CreateSettlement(const FString& Name, const FVector& Location, ESettlementType Type, int32 InitialPopulation)
{
    FSettlementData Settlement;
    Settlement.SettlementName = Name;
    Settlement.Location = Location;
    Settlement.SettlementType = Type;
    Settlement.Population = InitialPopulation;
    Settlement.Prosperity = 0.5f; // Neutral prosperity
    Settlement.Security = 0.6f; // Moderate security
    Settlement.Happiness = 0.5f; // Neutral happiness
    Settlement.ResourceAvailability = 0.7f; // Good resources
    
    // Set specializations based on settlement type and name
    if (Name == TEXT("Millhaven"))
    {
        Settlement.Specializations.Add(ESettlementSpecialization::Agriculture);
    }
    else if (Name == TEXT("Ironhold"))
    {
        Settlement.Specializations.Add(ESettlementSpecialization::Mining);
        Settlement.Specializations.Add(ESettlementSpecialization::Crafting);
    }
    else if (Name == TEXT("Goldport"))
    {
        Settlement.Specializations.Add(ESettlementSpecialization::Trade);
        Settlement.Specializations.Add(ESettlementSpecialization::Fishing);
    }
    else if (Name == TEXT("Oakstead"))
    {
        Settlement.Specializations.Add(ESettlementSpecialization::Trade);
        Settlement.Specializations.Add(ESettlementSpecialization::Crafting);
    }
    else if (Name == TEXT("Riverside"))
    {
        Settlement.Specializations.Add(ESettlementSpecialization::Fishing);
        Settlement.Specializations.Add(ESettlementSpecialization::Agriculture);
    }
    
    Settlements.Add(Name, Settlement);
    
    // Populate settlement with NPCs
    PopulateSettlement(Name, InitialPopulation);
    
    UE_LOG(LogTemp, Log, TEXT("Created settlement: %s (Type: %d, Population: %d)"), 
        *Name, (int32)Type, InitialPopulation);
}

void UNPCEcosystemSubsystem::PopulateSettlement(const FString& SettlementName, int32 PopulationCount)
{
    FSettlementData* Settlement = Settlements.Find(SettlementName);
    if (!Settlement)
        return;
    
    for (int32 i = 0; i < PopulationCount; i++)
    {
        FNPCData NewNPC = GenerateNPC(SettlementName, Settlement->SettlementType);
        NPCPopulation.Add(NewNPC.NPCID, NewNPC);
    }
    
    UE_LOG(LogTemp, Log, TEXT("Populated %s with %d NPCs"), *SettlementName, PopulationCount);
}

FNPCData UNPCEcosystemSubsystem::GenerateNPC(const FString& SettlementName, ESettlementType SettlementType)
{
    FNPCData NewNPC;
    NewNPC.NPCID = FGuid::NewGuid();
    NewNPC.Name = GenerateNPCName();
    NewNPC.HomeSettlement = SettlementName;
    NewNPC.CurrentSettlement = SettlementName;
    
    // Assign profession based on settlement specializations
    NewNPC.Profession = AssignProfession(SettlementType);
    
    // Generate personality traits
    NewNPC.PersonalityTraits.Sociability = FMath::FRandRange(0.0f, 1.0f);
    NewNPC.PersonalityTraits.Ambition = FMath::FRandRange(0.0f, 1.0f);
    NewNPC.PersonalityTraits.Loyalty = FMath::FRandRange(0.3f, 1.0f); // Most NPCs are somewhat loyal
    NewNPC.PersonalityTraits.Courage = FMath::FRandRange(0.0f, 1.0f);
    NewNPC.PersonalityTraits.Intelligence = FMath::FRandRange(0.2f, 1.0f);
    
    // Initialize needs
    NewNPC.Needs.Hunger = FMath::FRandRange(60.0f, 90.0f);
    NewNPC.Needs.Thirst = FMath::FRandRange(60.0f, 90.0f);
    NewNPC.Needs.Rest = FMath::FRandRange(60.0f, 90.0f);
    NewNPC.Needs.Safety = FMath::FRandRange(50.0f, 80.0f);
    NewNPC.Needs.Social = FMath::FRandRange(40.0f, 80.0f);
    NewNPC.Needs.Purpose = FMath::FRandRange(50.0f, 90.0f);
    
    // Initialize relationships (will be populated later)
    NewNPC.Relationships.Empty();
    
    // Set initial state
    NewNPC.CurrentActivity = ENPCActivity::Working;
    NewNPC.Mood = ENPCMood::Content;
    NewNPC.HealthStatus = ENPCHealthStatus::Healthy;
    
    // Generate daily schedule
    GenerateDailySchedule(NewNPC);
    
    return NewNPC;
}

FString UNPCEcosystemSubsystem::GenerateNPCName()
{
    TArray<FString> FirstNames = {
        TEXT("Aldric"), TEXT("Brenna"), TEXT("Cedric"), TEXT("Dara"), TEXT("Ewan"),
        TEXT("Fiona"), TEXT("Gareth"), TEXT("Hilda"), TEXT("Ivan"), TEXT("Jora"),
        TEXT("Kael"), TEXT("Lyra"), TEXT("Magnus"), TEXT("Nora"), TEXT("Osric"),
        TEXT("Petra"), TEXT("Quinn"), TEXT("Rhea"), TEXT("Soren"), TEXT("Thea")
    };
    
    TArray<FString> LastNames = {
        TEXT("Ironforge"), TEXT("Goldleaf"), TEXT("Stormwind"), TEXT("Brightblade"), TEXT("Darkwood"),
        TEXT("Silverstone"), TEXT("Redmane"), TEXT("Blackwater"), TEXT("Greycloak"), TEXT("Whitehawk")
    };
    
    FString FirstName = FirstNames[FMath::RandRange(0, FirstNames.Num() - 1)];
    FString LastName = LastNames[FMath::RandRange(0, LastNames.Num() - 1)];
    
    return FString::Printf(TEXT("%s %s"), *FirstName, *LastName);
}

ENPCProfession UNPCEcosystemSubsystem::AssignProfession(ESettlementType SettlementType)
{
    TArray<ENPCProfession> PossibleProfessions;
    
    // Base professions available everywhere
    PossibleProfessions.Add(ENPCProfession::Farmer);
    PossibleProfessions.Add(ENPCProfession::Merchant);
    PossibleProfessions.Add(ENPCProfession::Guard);
    PossibleProfessions.Add(ENPCProfession::Innkeeper);
    
    // Add specialized professions based on settlement type
    switch (SettlementType)
    {
    case ESettlementType::Village:
        PossibleProfessions.Add(ENPCProfession::Miller);
        PossibleProfessions.Add(ENPCProfession::Herbalist);
        break;
        
    case ESettlementType::Town:
        PossibleProfessions.Add(ENPCProfession::Blacksmith);
        PossibleProfessions.Add(ENPCProfession::Scholar);
        PossibleProfessions.Add(ENPCProfession::Priest);
        break;
        
    case ESettlementType::City:
        PossibleProfessions.Add(ENPCProfession::Noble);
        PossibleProfessions.Add(ENPCProfession::Blacksmith);
        PossibleProfessions.Add(ENPCProfession::Scholar);
        PossibleProfessions.Add(ENPCProfession::Priest);
        PossibleProfessions.Add(ENPCProfession::Miner);
        break;
        
    case ESettlementType::Port:
        PossibleProfessions.Add(ENPCProfession::Fisherman);
        PossibleProfessions.Add(ENPCProfession::Sailor);
        PossibleProfessions.Add(ENPCProfession::Merchant);
        break;
    }
    
    return PossibleProfessions[FMath::RandRange(0, PossibleProfessions.Num() - 1)];
}

void UNPCEcosystemSubsystem::GenerateDailySchedule(FNPCData& NPC)
{
    NPC.DailySchedule.Empty();
    
    // Create a basic daily schedule based on profession
    switch (NPC.Profession)
    {
    case ENPCProfession::Farmer:
        NPC.DailySchedule.Add(FScheduleEntry{6.0f, 8.0f, ENPCActivity::Sleeping, TEXT("Home")});
        NPC.DailySchedule.Add(FScheduleEntry{8.0f, 12.0f, ENPCActivity::Working, TEXT("Fields")});
        NPC.DailySchedule.Add(FScheduleEntry{12.0f, 13.0f, ENPCActivity::Eating, TEXT("Home")});
        NPC.DailySchedule.Add(FScheduleEntry{13.0f, 17.0f, ENPCActivity::Working, TEXT("Fields")});
        NPC.DailySchedule.Add(FScheduleEntry{17.0f, 19.0f, ENPCActivity::Socializing, TEXT("Village Center")});
        NPC.DailySchedule.Add(FScheduleEntry{19.0f, 20.0f, ENPCActivity::Eating, TEXT("Home")});
        NPC.DailySchedule.Add(FScheduleEntry{20.0f, 22.0f, ENPCActivity::Relaxing, TEXT("Home")});
        NPC.DailySchedule.Add(FScheduleEntry{22.0f, 6.0f, ENPCActivity::Sleeping, TEXT("Home")});
        break;
        
    case ENPCProfession::Merchant:
        NPC.DailySchedule.Add(FScheduleEntry{6.0f, 8.0f, ENPCActivity::Sleeping, TEXT("Home")});
        NPC.DailySchedule.Add(FScheduleEntry{8.0f, 12.0f, ENPCActivity::Trading, TEXT("Market")});
        NPC.DailySchedule.Add(FScheduleEntry{12.0f, 13.0f, ENPCActivity::Eating, TEXT("Tavern")});
        NPC.DailySchedule.Add(FScheduleEntry{13.0f, 17.0f, ENPCActivity::Trading, TEXT("Market")});
        NPC.DailySchedule.Add(FScheduleEntry{17.0f, 19.0f, ENPCActivity::Socializing, TEXT("Tavern")});
        NPC.DailySchedule.Add(FScheduleEntry{19.0f, 20.0f, ENPCActivity::Eating, TEXT("Tavern")});
        NPC.DailySchedule.Add(FScheduleEntry{20.0f, 22.0f, ENPCActivity::Relaxing, TEXT("Home")});
        NPC.DailySchedule.Add(FScheduleEntry{22.0f, 6.0f, ENPCActivity::Sleeping, TEXT("Home")});
        break;
        
    case ENPCProfession::Guard:
        // Guards work in shifts
        if (FMath::RandBool()) // Day shift
        {
            NPC.DailySchedule.Add(FScheduleEntry{22.0f, 8.0f, ENPCActivity::Sleeping, TEXT("Barracks")});
            NPC.DailySchedule.Add(FScheduleEntry{8.0f, 12.0f, ENPCActivity::Patrolling, TEXT("Settlement")});
            NPC.DailySchedule.Add(FScheduleEntry{12.0f, 13.0f, ENPCActivity::Eating, TEXT("Barracks")});
            NPC.DailySchedule.Add(FScheduleEntry{13.0f, 17.0f, ENPCActivity::Patrolling, TEXT("Settlement")});
            NPC.DailySchedule.Add(FScheduleEntry{17.0f, 19.0f, ENPCActivity::Training, TEXT("Training Grounds")});
            NPC.DailySchedule.Add(FScheduleEntry{19.0f, 20.0f, ENPCActivity::Eating, TEXT("Tavern")});
            NPC.DailySchedule.Add(FScheduleEntry{20.0f, 22.0f, ENPCActivity::Socializing, TEXT("Tavern")});
        }
        else // Night shift
        {
            NPC.DailySchedule.Add(FScheduleEntry{8.0f, 16.0f, ENPCActivity::Sleeping, TEXT("Barracks")});
            NPC.DailySchedule.Add(FScheduleEntry{16.0f, 17.0f, ENPCActivity::Eating, TEXT("Barracks")});
            NPC.DailySchedule.Add(FScheduleEntry{17.0f, 19.0f, ENPCActivity::Training, TEXT("Training Grounds")});
            NPC.DailySchedule.Add(FScheduleEntry{19.0f, 20.0f, ENPCActivity::Eating, TEXT("Tavern")});
            NPC.DailySchedule.Add(FScheduleEntry{20.0f, 8.0f, ENPCActivity::Patrolling, TEXT("Settlement")});
        }
        break;
        
    default:
        // Generic schedule for other professions
        NPC.DailySchedule.Add(FScheduleEntry{22.0f, 7.0f, ENPCActivity::Sleeping, TEXT("Home")});
        NPC.DailySchedule.Add(FScheduleEntry{7.0f, 8.0f, ENPCActivity::Eating, TEXT("Home")});
        NPC.DailySchedule.Add(FScheduleEntry{8.0f, 12.0f, ENPCActivity::Working, TEXT("Workplace")});
        NPC.DailySchedule.Add(FScheduleEntry{12.0f, 13.0f, ENPCActivity::Eating, TEXT("Home")});
        NPC.DailySchedule.Add(FScheduleEntry{13.0f, 17.0f, ENPCActivity::Working, TEXT("Workplace")});
        NPC.DailySchedule.Add(FScheduleEntry{17.0f, 19.0f, ENPCActivity::Socializing, TEXT("Village Center")});
        NPC.DailySchedule.Add(FScheduleEntry{19.0f, 20.0f, ENPCActivity::Eating, TEXT("Home")});
        NPC.DailySchedule.Add(FScheduleEntry{20.0f, 22.0f, ENPCActivity::Relaxing, TEXT("Home")});
        break;
    }
}

void UNPCEcosystemSubsystem::InitializeNPCArchetypes()
{
    // Define behavioral archetypes that influence NPC decision making
    
    FNPCArchetype Industrious;
    Industrious.ArchetypeName = TEXT("Industrious");
    Industrious.WorkEfficiencyModifier = 1.3f;
    Industrious.SocialNeedModifier = 0.8f;
    Industrious.RestNeedModifier = 1.2f;
    NPCArchetypes.Add(TEXT("Industrious"), Industrious);
    
    FNPCArchetype Social;
    Social.ArchetypeName = TEXT("Social");
    Social.WorkEfficiencyModifier = 0.9f;
    Social.SocialNeedModifier = 1.5f;
    Social.RestNeedModifier = 0.9f;
    NPCArchetypes.Add(TEXT("Social"), Social);
    
    FNPCArchetype Cautious;
    Cautious.ArchetypeName = TEXT("Cautious");
    Cautious.WorkEfficiencyModifier = 1.0f;
    Cautious.SocialNeedModifier = 0.7f;
    Cautious.RestNeedModifier = 1.1f;
    NPCArchetypes.Add(TEXT("Cautious"), Cautious);
    
    FNPCArchetype Adventurous;
    Adventurous.ArchetypeName = TEXT("Adventurous");
    Adventurous.WorkEfficiencyModifier = 0.8f;
    Adventurous.SocialNeedModifier = 1.2f;
    Adventurous.RestNeedModifier = 0.8f;
    NPCArchetypes.Add(TEXT("Adventurous"), Adventurous);
    
    UE_LOG(LogTemp, Log, TEXT("Initialized %d NPC archetypes"), NPCArchetypes.Num());
}

void UNPCEcosystemSubsystem::InitializeSocialNetworks()
{
    // Create social connections between NPCs
    for (auto& NPCPair : NPCPopulation)
    {
        FNPCData& NPC = NPCPair.Value;
        CreateSocialConnections(NPC);
    }
    
    UE_LOG(LogTemp, Log, TEXT("Initialized social networks for %d NPCs"), NPCPopulation.Num());
}

void UNPCEcosystemSubsystem::CreateSocialConnections(FNPCData& NPC)
{
    // Find other NPCs in the same settlement
    TArray<FGuid> PotentialConnections;
    
    for (const auto& OtherNPCPair : NPCPopulation)
    {
        const FNPCData& OtherNPC = OtherNPCPair.Value;
        
        if (OtherNPC.NPCID != NPC.NPCID && OtherNPC.CurrentSettlement == NPC.CurrentSettlement)
        {
            PotentialConnections.Add(OtherNPC.NPCID);
        }
    }
    
    // Create relationships based on personality and profession compatibility
    int32 MaxConnections = FMath::RandRange(2, 8); // Each NPC has 2-8 relationships
    int32 ConnectionsToCreate = FMath::Min(MaxConnections, PotentialConnections.Num());
    
    for (int32 i = 0; i < ConnectionsToCreate; i++)
    {
        if (PotentialConnections.Num() == 0)
            break;
            
        int32 RandomIndex = FMath::RandRange(0, PotentialConnections.Num() - 1);
        FGuid TargetNPCID = PotentialConnections[RandomIndex];
        PotentialConnections.RemoveAt(RandomIndex);
        
        // Determine relationship type and strength
        ERelationshipType RelType = DetermineRelationshipType(NPC, *NPCPopulation.Find(TargetNPCID));
        float RelStrength = FMath::FRandRange(0.3f, 0.9f);
        
        FNPCRelationship Relationship;
        Relationship.TargetNPCID = TargetNPCID;
        Relationship.RelationshipType = RelType;
        Relationship.RelationshipStrength = RelStrength;
        Relationship.LastInteractionTime = 0.0f;
        
        NPC.Relationships.Add(TargetNPCID, Relationship);
        
        // Create reciprocal relationship
        if (FNPCData* TargetNPC = NPCPopulation.Find(TargetNPCID))
        {
            FNPCRelationship ReciprocalRelationship;
            ReciprocalRelationship.TargetNPCID = NPC.NPCID;
            ReciprocalRelationship.RelationshipType = RelType;
            ReciprocalRelationship.RelationshipStrength = RelStrength;
            ReciprocalRelationship.LastInteractionTime = 0.0f;
            
            TargetNPC->Relationships.Add(NPC.NPCID, ReciprocalRelationship);
        }
    }
}

ERelationshipType UNPCEcosystemSubsystem::DetermineRelationshipType(const FNPCData& NPC1, const FNPCData& NPC2)
{
    // Base chance for different relationship types
    float FriendChance = 0.4f;
    float RivalChance = 0.1f;
    float RomanticChance = 0.05f;
    
    // Modify chances based on personality compatibility
    float PersonalityCompatibility = CalculatePersonalityCompatibility(NPC1, NPC2);
    
    FriendChance += PersonalityCompatibility * 0.3f;
    RivalChance -= PersonalityCompatibility * 0.05f;
    RomanticChance += PersonalityCompatibility * 0.1f;
    
    // Profession-based modifiers
    if (NPC1.Profession == NPC2.Profession)
    {
        FriendChance += 0.2f; // Same profession = more likely to be friends
        RivalChance += 0.1f; // But also potential for professional rivalry
    }
    
    // Determine relationship type
    float RandomValue = FMath::FRand();
    
    if (RandomValue < RomanticChance)
    {
        return ERelationshipType::Romantic;
    }
    else if (RandomValue < RomanticChance + RivalChance)
    {
        return ERelationshipType::Rival;
    }
    else if (RandomValue < RomanticChance + RivalChance + FriendChance)
    {
        return ERelationshipType::Friend;
    }
    else
    {
        return ERelationshipType::Acquaintance;
    }
}

float UNPCEcosystemSubsystem::CalculatePersonalityCompatibility(const FNPCData& NPC1, const FNPCData& NPC2)
{
    // Calculate compatibility based on personality traits
    float SociabilityDiff = FMath::Abs(NPC1.PersonalityTraits.Sociability - NPC2.PersonalityTraits.Sociability);
    float AmbitionDiff = FMath::Abs(NPC1.PersonalityTraits.Ambition - NPC2.PersonalityTraits.Ambition);
    float LoyaltyDiff = FMath::Abs(NPC1.PersonalityTraits.Loyalty - NPC2.PersonalityTraits.Loyalty);
    
    // Lower differences = higher compatibility
    float Compatibility = 1.0f - ((SociabilityDiff + AmbitionDiff + LoyaltyDiff) / 3.0f);
    
    return FMath::Clamp(Compatibility, 0.0f, 1.0f);
}

void UNPCEcosystemSubsystem::UpdateEcosystem()
{
    UpdateSettlementConditions();
    UpdatePopulationDynamics();
    ProcessNPCLifeEvents();
    
    UE_LOG(LogTemp, Log, TEXT("Ecosystem updated - Total NPCs: %d"), NPCPopulation.Num());
}

void UNPCEcosystemSubsystem::UpdateSettlementConditions()
{
    for (auto& SettlementPair : Settlements)
    {
        FSettlementData& Settlement = SettlementPair.Value;
        
        // Update prosperity based on economic factors
        float EconomicFactor = CalculateEconomicFactor(Settlement);
        Settlement.Prosperity = FMath::Lerp(Settlement.Prosperity, EconomicFactor, 0.1f);
        
        // Update security based on guard presence and external threats
        float SecurityFactor = CalculateSecurityFactor(Settlement);
        Settlement.Security = FMath::Lerp(Settlement.Security, SecurityFactor, 0.1f);
        
        // Update happiness based on prosperity, security, and population needs
        float HappinessFactor = (Settlement.Prosperity + Settlement.Security + Settlement.ResourceAvailability) / 3.0f;
        Settlement.Happiness = FMath::Lerp(Settlement.Happiness, HappinessFactor, 0.1f);
        
        // Clamp all values
        Settlement.Prosperity = FMath::Clamp(Settlement.Prosperity, 0.0f, 1.0f);
        Settlement.Security = FMath::Clamp(Settlement.Security, 0.0f, 1.0f);
        Settlement.Happiness = FMath::Clamp(Settlement.Happiness, 0.0f, 1.0f);
        Settlement.ResourceAvailability = FMath::Clamp(Settlement.ResourceAvailability, 0.0f, 1.0f);
        
        UE_LOG(LogTemp, Log, TEXT("Settlement %s - Prosperity: %.2f, Security: %.2f, Happiness: %.2f"), 
            *Settlement.SettlementName, Settlement.Prosperity, Settlement.Security, Settlement.Happiness);
    }
}

float UNPCEcosystemSubsystem::CalculateEconomicFactor(const FSettlementData& Settlement)
{
    float EconomicFactor = 0.5f; // Base economic level
    
    // Specialization bonuses
    for (ESettlementSpecialization Specialization : Settlement.Specializations)
    {
        switch (Specialization)
        {
        case ESettlementSpecialization::Trade:
            EconomicFactor += 0.2f;
            break;
        case ESettlementSpecialization::Crafting:
            EconomicFactor += 0.15f;
            break;
        case ESettlementSpecialization::Mining:
            EconomicFactor += 0.1f;
            break;
        case ESettlementSpecialization::Agriculture:
            EconomicFactor += 0.1f;
            break;
        case ESettlementSpecialization::Fishing:
            EconomicFactor += 0.05f;
            break;
        }
    }
    
    // Population size factor (larger settlements have economic advantages)
    float PopulationFactor = FMath::Min(1.0f, Settlement.Population / 300.0f);
    EconomicFactor += PopulationFactor * 0.2f;
    
    // Random economic events
    if (FMath::FRand() < 0.1f) // 10% chance of economic event
    {
        if (FMath::FRand() < 0.7f) // 70% chance of positive event
        {
            EconomicFactor += FMath::FRandRange(0.05f, 0.15f);
            UE_LOG(LogTemp, Log, TEXT("Positive economic event in %s"), *Settlement.SettlementName);
        }
        else // 30% chance of negative event
        {
            EconomicFactor -= FMath::FRandRange(0.05f, 0.15f);
            UE_LOG(LogTemp, Log, TEXT("Negative economic event in %s"), *Settlement.SettlementName);
        }
    }
    
    return FMath::Clamp(EconomicFactor, 0.0f, 1.0f);
}

float UNPCEcosystemSubsystem::CalculateSecurityFactor(const FSettlementData& Settlement)
{
    float SecurityFactor = 0.5f; // Base security level
    
    // Count guards in settlement
    int32 GuardCount = 0;
    for (const auto& NPCPair : NPCPopulation)
    {
        const FNPCData& NPC = NPCPair.Value;
        if (NPC.CurrentSettlement == Settlement.SettlementName && NPC.Profession == ENPCProfession::Guard)
        {
            GuardCount++;
        }
    }
    
    // Security based on guard ratio
    float GuardRatio = static_cast<float>(GuardCount) / FMath::Max(1.0f, static_cast<float>(Settlement.Population));
    SecurityFactor += GuardRatio * 2.0f; // Guards significantly improve security
    
    // Settlement type modifiers
    switch (Settlement.SettlementType)
    {
    case ESettlementType::City:
        SecurityFactor += 0.2f; // Cities have better defenses
        break;
    case ESettlementType::Town:
        SecurityFactor += 0.1f; // Towns have moderate defenses
        break;
    case ESettlementType::Village:
        SecurityFactor -= 0.1f; // Villages are more vulnerable
        break;
    case ESettlementType::Port:
        SecurityFactor += 0.05f; // Ports have some naval protection
        break;
    }
    
    return FMath::Clamp(SecurityFactor, 0.0f, 1.0f);
}

void UNPCEcosystemSubsystem::UpdatePopulationDynamics()
{
    // Handle population growth, migration, and death
    for (auto& SettlementPair : Settlements)
    {
        FSettlementData& Settlement = SettlementPair.Value;
        
        // Population growth based on happiness and prosperity
        if (Settlement.Happiness > 0.6f && Settlement.Prosperity > 0.5f)
        {
            if (FMath::FRand() < PopulationGrowthRate && Settlement.Population < MaxPopulationPerSettlement)
            {
                // Add new NPC
                FNPCData NewNPC = GenerateNPC(Settlement.SettlementName, Settlement.SettlementType);
                NPCPopulation.Add(NewNPC.NPCID, NewNPC);
                Settlement.Population++;
                
                UE_LOG(LogTemp, Log, TEXT("Population growth in %s - New population: %d"),
                    *Settlement.SettlementName, Settlement.Population);
            }
        }
    }
}

void UNPCEcosystemSubsystem::ProcessNPCLifeEvents()
{
    // Process major life events for NPCs
    for (auto& NPCPair : NPCPopulation)
    {
        FNPCData& NPC = NPCPair.Value;
        
        // Random life events
        if (FMath::FRand() < 0.001f) // Very rare events
        {
            ProcessLifeEvent(NPC);
        }
        
        // Update NPC needs over time
        UpdateNPCNeeds(NPC);
    }
}

void UNPCEcosystemSubsystem::ProcessLifeEvent(FNPCData& NPC)
{
    // Determine type of life event
    float EventRoll = FMath::FRand();
    
    if (EventRoll < 0.3f)
    {
        // Career change
        ENPCProfession NewProfession = AssignProfession(ESettlementType::Village); // Use generic assignment
        if (NewProfession != NPC.Profession)
        {
            NPC.Profession = NewProfession;
            GenerateDailySchedule(NPC); // Update schedule for new profession
            UE_LOG(LogTemp, Log, TEXT("NPC %s changed profession"), *NPC.Name);
        }
    }
    else if (EventRoll < 0.6f)
    {
        // Mood change
        TArray<ENPCMood> PossibleMoods = {ENPCMood::Happy, ENPCMood::Content, ENPCMood::Sad, ENPCMood::Angry, ENPCMood::Excited};
        NPC.Mood = PossibleMoods[FMath::RandRange(0, PossibleMoods.Num() - 1)];
        UE_LOG(LogTemp, Log, TEXT("NPC %s mood changed"), *NPC.Name);
    }
    else if (EventRoll < 0.9f)
    {
        // Health event
        TArray<ENPCHealthStatus> PossibleHealth = {ENPCHealthStatus::Healthy, ENPCHealthStatus::Sick, ENPCHealthStatus::Injured};
        NPC.HealthStatus = PossibleHealth[FMath::RandRange(0, PossibleHealth.Num() - 1)];
        UE_LOG(LogTemp, Log, TEXT("NPC %s health status changed"), *NPC.Name);
    }
}

void UNPCEcosystemSubsystem::UpdateNPCNeeds(FNPCData& NPC)
{
    // Gradually decrease needs over time (they get hungry, tired, etc.)
    NPC.Needs.Hunger = FMath::Max(0.0f, NPC.Needs.Hunger - 0.5f);
    NPC.Needs.Thirst = FMath::Max(0.0f, NPC.Needs.Thirst - 0.7f);
    NPC.Needs.Rest = FMath::Max(0.0f, NPC.Needs.Rest - 0.3f);
    NPC.Needs.Social = FMath::Max(0.0f, NPC.Needs.Social - 0.2f);
    
    // Safety and purpose are more stable
    if (FMath::FRand() < 0.1f)
    {
        NPC.Needs.Safety = FMath::Max(0.0f, NPC.Needs.Safety - 0.1f);
        NPC.Needs.Purpose = FMath::Max(0.0f, NPC.Needs.Purpose - 0.1f);
    }
}

void UNPCEcosystemSubsystem::UpdateSocialInteractions()
{
    // Update relationships and social dynamics
    for (auto& NPCPair : NPCPopulation)
    {
        FNPCData& NPC = NPCPair.Value;
        UpdateNPCRelationships(NPC);
    }
    
    UE_LOG(LogTemp, Log, TEXT("Updated social interactions for %d NPCs"), NPCPopulation.Num());
}

void UNPCEcosystemSubsystem::UpdateNPCRelationships(FNPCData& NPC)
{
    // Update existing relationships
    for (auto& RelationshipPair : NPC.Relationships)
    {
        FNPCRelationship& Relationship = RelationshipPair.Value;
        
        // Relationships naturally decay over time without interaction
        Relationship.LastInteractionTime += 1.0f; // Increment by update interval
        
        if (Relationship.LastInteractionTime > 30.0f) // 30 time units without interaction
        {
            Relationship.RelationshipStrength = FMath::Max(0.1f, Relationship.RelationshipStrength - 0.01f);
        }
        
        // Chance for random interaction
        if (FMath::FRand() < 0.1f) // 10% chance per update
        {
            ProcessSocialInteraction(NPC, Relationship);
        }
    }
}

void UNPCEcosystemSubsystem::ProcessSocialInteraction(FNPCData& NPC, FNPCRelationship& Relationship)
{
    // Reset interaction timer
    Relationship.LastInteractionTime = 0.0f;
    
    // Determine interaction outcome based on relationship type and personalities
    float InteractionOutcome = FMath::FRandRange(-0.1f, 0.1f);
    
    // Modify based on relationship type
    switch (Relationship.RelationshipType)
    {
    case ERelationshipType::Friend:
        InteractionOutcome += 0.05f; // Friends tend to have positive interactions
        break;
    case ERelationshipType::Rival:
        InteractionOutcome -= 0.05f; // Rivals tend to have negative interactions
        break;
    case ERelationshipType::Romantic:
        InteractionOutcome += 0.1f; // Romantic relationships are usually positive
        break;
    case ERelationshipType::Acquaintance:
        // No modifier - neutral
        break;
    }
    
    // Apply interaction outcome
    Relationship.RelationshipStrength = FMath::Clamp(
        Relationship.RelationshipStrength + InteractionOutcome,
        0.0f, 1.0f
    );
    
    // Update NPC's social need based on interaction
    NPC.Needs.Social = FMath::Min(100.0f, NPC.Needs.Social + 5.0f);
}

void UNPCEcosystemSubsystem::UpdateMigrationPatterns()
{
    // Handle NPCs moving between settlements
    TArray<FGuid> NPCsToMigrate;
    
    for (const auto& NPCPair : NPCPopulation)
    {
        const FNPCData& NPC = NPCPair.Value;
        
        // Check if NPC wants to migrate
        if (ShouldNPCMigrate(NPC))
        {
            NPCsToMigrate.Add(NPC.NPCID);
        }
    }
    
    // Process migrations
    for (const FGuid& NPCID : NPCsToMigrate)
    {
        ProcessNPCMigration(NPCID);
    }
    
    if (NPCsToMigrate.Num() > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("Processed %d NPC migrations"), NPCsToMigrate.Num());
    }
}

bool UNPCEcosystemSubsystem::ShouldNPCMigrate(const FNPCData& NPC)
{
    // Get current settlement data
    const FSettlementData* CurrentSettlement = Settlements.Find(NPC.CurrentSettlement);
    if (!CurrentSettlement)
        return false;
    
    // Migration factors
    float MigrationDesire = 0.0f;
    
    // Unhappiness in current settlement
    if (CurrentSettlement->Happiness < MigrationThreshold)
    {
        MigrationDesire += 0.3f;
    }
    
    // Low prosperity
    if (CurrentSettlement->Prosperity < MigrationThreshold)
    {
        MigrationDesire += 0.2f;
    }
    
    // Low security
    if (CurrentSettlement->Security < MigrationThreshold)
    {
        MigrationDesire += 0.2f;
    }
    
    // Personality factors
    if (NPC.PersonalityTraits.Ambition > 0.7f)
    {
        MigrationDesire += 0.1f; // Ambitious NPCs more likely to migrate
    }
    
    if (NPC.PersonalityTraits.Loyalty > 0.8f)
    {
        MigrationDesire -= 0.2f; // Loyal NPCs less likely to leave
    }
    
    // Random factor
    MigrationDesire += FMath::FRandRange(-0.1f, 0.1f);
    
    return MigrationDesire > 0.5f && FMath::FRand() < 0.05f; // 5% chance if conditions are met
}

void UNPCEcosystemSubsystem::ProcessNPCMigration(const FGuid& NPCID)
{
    FNPCData* NPC = NPCPopulation.Find(NPCID);
    if (!NPC)
        return;
    
    // Find best destination settlement
    FString BestDestination;
    float BestScore = -1.0f;
    
    for (const auto& SettlementPair : Settlements)
    {
        const FSettlementData& Settlement = SettlementPair.Value;
        
        // Don't migrate to current settlement
        if (Settlement.SettlementName == NPC->CurrentSettlement)
            continue;
        
        // Calculate settlement attractiveness
        float Score = (Settlement.Happiness + Settlement.Prosperity + Settlement.Security) / 3.0f;
        
        // Prefer settlements with room for growth
        if (Settlement.Population < MaxPopulationPerSettlement * 0.8f)
        {
            Score += 0.1f;
        }
        
        if (Score > BestScore)
        {
            BestScore = Score;
            BestDestination = Settlement.SettlementName;
        }
    }
    
    // Perform migration
    if (!BestDestination.IsEmpty() && BestScore > 0.4f)
    {
        FString OldSettlement = NPC->CurrentSettlement;
        NPC->CurrentSettlement = BestDestination;
        
        // Update settlement populations
        if (FSettlementData* OldSettlementData = Settlements.Find(OldSettlement))
        {
            OldSettlementData->Population = FMath::Max(0, OldSettlementData->Population - 1);
        }
        
        if (FSettlementData* NewSettlementData = Settlements.Find(BestDestination))
        {
            NewSettlementData->Population++;
        }
        
        UE_LOG(LogTemp, Log, TEXT("NPC %s migrated from %s to %s"),
            *NPC->Name, *OldSettlement, *BestDestination);
    }
}

void UNPCEcosystemSubsystem::UpdateNPCBehaviors(float DeltaTime)
{
    // Update individual NPC behaviors and activities
    for (auto& NPCPair : NPCPopulation)
    {
        FNPCData& NPC = NPCPair.Value;
        UpdateNPCActivity(NPC, DeltaTime);
    }
}

void UNPCEcosystemSubsystem::UpdateNPCActivity(FNPCData& NPC, float DeltaTime)
{
    // Simple time-based activity system
    // In a full implementation, this would be much more sophisticated
    
    // Get current time of day (simplified - using a 24-hour cycle)
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    float CurrentTime = FMath::Fmod(World->GetTimeSeconds() / 60.0f, 24.0f); // Convert to hours
    
    // Find appropriate activity for current time
    ENPCActivity NewActivity = ENPCActivity::Relaxing; // Default
    
    for (const FScheduleEntry& Entry : NPC.DailySchedule)
    {
        if (CurrentTime >= Entry.StartTime && CurrentTime < Entry.EndTime)
        {
            NewActivity = Entry.Activity;
            break;
        }
    }
    
    // Update activity if it changed
    if (NewActivity != NPC.CurrentActivity)
    {
        NPC.CurrentActivity = NewActivity;
        
        // Activity-specific need updates
        switch (NewActivity)
        {
        case ENPCActivity::Eating:
            NPC.Needs.Hunger = FMath::Min(100.0f, NPC.Needs.Hunger + 30.0f);
            NPC.Needs.Thirst = FMath::Min(100.0f, NPC.Needs.Thirst + 20.0f);
            break;
        case ENPCActivity::Sleeping:
            NPC.Needs.Rest = FMath::Min(100.0f, NPC.Needs.Rest + 40.0f);
            break;
        case ENPCActivity::Socializing:
            NPC.Needs.Social = FMath::Min(100.0f, NPC.Needs.Social + 15.0f);
            break;
        case ENPCActivity::Working:
            NPC.Needs.Purpose = FMath::Min(100.0f, NPC.Needs.Purpose + 10.0f);
            break;
        }
    }
}