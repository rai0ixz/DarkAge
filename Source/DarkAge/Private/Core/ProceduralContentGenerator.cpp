#include "Core/ProceduralContentGenerator.h"
#include "Kismet/KismetMathLibrary.h"

UProceduralContentGenerator::UProceduralContentGenerator()
{
    RandomSeed = 12345; // Default seed
    RandomStream.Initialize(RandomSeed);
}

void UProceduralContentGenerator::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    InitializeContentTemplates();
}

void UProceduralContentGenerator::Deinitialize()
{
    Super::Deinitialize();
}

void UProceduralContentGenerator::InitializeContentTemplates()
{
    // Location types
    LocationTypes = {
        "Village", "Town", "Castle", "Fortress", "Monastery", "Farmstead",
        "Mine", "Quarry", "Forest Clearing", "Cave", "Ruins", "Outpost"
    };

    // Location features
    LocationFeatures = {
        "River", "Lake", "Forest", "Mountains", "Mine", "Farm", "Mill",
        "Blacksmith", "Tavern", "Temple", "Market", "Walls", "Tower", "Bridge"
    };

    // Quest types
    QuestTypes = {
        "Delivery", "Combat", "Exploration", "Social", "Crafting", "Mystery"
    };

    // NPC names
    NPCNames = {
        "Aldric", "Beatrice", "Cedric", "Daphne", "Edmund", "Fiona",
        "Gareth", "Helena", "Isaac", "Julia", "Kenneth", "Liora",
        "Marcus", "Nora", "Oliver", "Penelope", "Quentin", "Rosalind"
    };

    // NPC backgrounds
    NPCBackgrounds = {
        "former soldier", "merchant's child", "local farmer", "wandering scholar",
        "village healer", "blacksmith's apprentice", "innkeeper", "guard captain",
        "noble descendant", "mysterious traveler", "local priest", "hunter"
    };

    // Event types
    EventTypes = {
        "Bandit Attack", "Merchant Caravan", "Strange Occurrence", "Political Crisis",
        "Resource Discovery", "Festival", "Natural Disaster", "Royal Visit"
    };
}

FProceduralLocation UProceduralContentGenerator::GenerateLocation(const FVector& BasePosition, const FString& PreferredType)
{
    FProceduralLocation Location;

    // Choose location type
    if (!PreferredType.IsEmpty())
    {
        Location.LocationType = PreferredType;
    }
    else
    {
        Location.LocationType = GetRandomElement(LocationTypes);
    }

    // Generate name
    Location.LocationName = GenerateLocationName(Location.LocationType);

    // Generate position
    Location.Position = GenerateLocationPosition(BasePosition, 1000.0f, 5000.0f);

    // Generate features
    int32 FeatureCount = GetRandomInt(1, 4);
    Location.Features = GenerateLocationFeatures(Location.LocationType, FeatureCount);

    // Calculate danger and value
    Location.DangerLevel = CalculateLocationDanger(Location.LocationType, Location.Features);
    Location.EconomicValue = CalculateLocationValue(Location.LocationType, Location.Features);

    return Location;
}

TArray<FProceduralLocation> UProceduralContentGenerator::GenerateRegion(int32 LocationCount, const FVector& Center, float Radius)
{
    TArray<FProceduralLocation> Locations;

    for (int32 i = 0; i < LocationCount; ++i)
    {
        FProceduralLocation Location = GenerateLocation(Center);
        Locations.Add(Location);
    }

    return Locations;
}

FProceduralQuest UProceduralContentGenerator::GenerateQuest(const FString& QuestType, int32 Difficulty, const FVector& PlayerLocation)
{
    FProceduralQuest Quest;

    Quest.QuestID = FString::Printf(TEXT("Quest_%d_%s"), GetRandomInt(1000, 9999), *QuestType);
    Quest.QuestType = QuestType;
    Quest.Difficulty = Difficulty;

    // Generate title and description
    Quest.QuestTitle = GenerateQuestTitle(QuestType, Difficulty);
    Quest.QuestDescription = GenerateQuestDescription(Quest);

    // Generate objectives
    Quest.Objectives = GenerateQuestObjectives(QuestType, Difficulty);

    // Generate target location
    Quest.TargetLocation = GenerateLocationPosition(PlayerLocation, 2000.0f, 8000.0f);

    // Calculate reward
    Quest.RewardGold = CalculateQuestReward(Quest);

    // Generate reward items
    if (GetRandomBool(0.6f)) // 60% chance for item reward
    {
        Quest.RewardItems.Add("Random Valuable Item");
    }

    return Quest;
}

TArray<FProceduralQuest> UProceduralContentGenerator::GenerateQuestChain(int32 ChainLength, const FVector& StartLocation)
{
    TArray<FProceduralQuest> QuestChain;

    FVector CurrentLocation = StartLocation;

    for (int32 i = 0; i < ChainLength; ++i)
    {
        FString QuestType = GetRandomElement(QuestTypes);
        int32 Difficulty = FMath::Clamp(i + 1, 1, 5); // Increasing difficulty

        FProceduralQuest Quest = GenerateQuest(QuestType, Difficulty, CurrentLocation);
        QuestChain.Add(Quest);

        // Next quest starts near the previous one's target
        CurrentLocation = Quest.TargetLocation;
    }

    return QuestChain;
}

FProceduralNPC UProceduralContentGenerator::GenerateNPC(const FString& NPCType, int32 PowerLevel)
{
    FProceduralNPC NPC;

    NPC.NPCType = NPCType;
    NPC.NPCName = GenerateNPCName(NPCType);

    // Generate personality
    TArray<FString> Personalities = {"Brave", "Cowardly", "Greedy", "Generous", "Honest", "Deceitful"};
    NPC.PersonalityType = GetRandomElement(Personalities);

    // Generate background
    int32 BackgroundCount = GetRandomInt(1, 3);
    NPC.Background = GenerateNPCBackground(NPCType, BackgroundCount);

    // Generate skills
    NPC.Skills = GenerateNPCSkills(NPCType, PowerLevel);

    // Generate relationships
    NPC.Relationships = GenerateNPCRelationships(NPC);

    // Calculate wealth and influence
    NPC.Wealth = GetRandomInt(10, 100) * PowerLevel;
    NPC.Influence = GetRandomInt(1, 10) * PowerLevel;

    return NPC;
}

TArray<FProceduralNPC> UProceduralContentGenerator::GenerateSettlementNPCs(int32 NPCCount, const FString& SettlementType)
{
    TArray<FProceduralNPC> NPCs;

    TArray<FString> NPCTypes;
    if (SettlementType == "Village")
    {
        NPCTypes = {"Farmer", "Blacksmith", "Merchant", "Guard", "Priest"};
    }
    else if (SettlementType == "Castle")
    {
        NPCTypes = {"Noble", "Knight", "Guard", "Servant", "Scholar"};
    }
    else if (SettlementType == "Town")
    {
        NPCTypes = {"Merchant", "Guard", "Innkeeper", "Blacksmith", "Priest", "Scholar"};
    }

    for (int32 i = 0; i < NPCCount; ++i)
    {
        FString NPCType = GetRandomElement(NPCTypes);
        int32 PowerLevel = GetRandomInt(1, 3);
        FProceduralNPC NPC = GenerateNPC(NPCType, PowerLevel);
        NPCs.Add(NPC);
    }

    return NPCs;
}

FString UProceduralContentGenerator::GenerateRandomEvent(const FVector& Location)
{
    return GetRandomElement(EventTypes);
}

TArray<FString> UProceduralContentGenerator::GenerateEventChain(int32 EventCount)
{
    TArray<FString> Events;

    for (int32 i = 0; i < EventCount; ++i)
    {
        Events.Add(GenerateRandomEvent(FVector::ZeroVector));
    }

    return Events;
}

void UProceduralContentGenerator::SeedRandomGenerator(int32 Seed)
{
    RandomSeed = Seed;
    RandomStream.Initialize(RandomSeed);
}

// Private helper functions
FString UProceduralContentGenerator::GenerateLocationName(const FString& Type)
{
    TArray<FString> Prefixes = {"North", "South", "East", "West", "High", "Low", "Old", "New"};
    TArray<FString> Suffixes = {"ville", "town", "burg", "ford", "ham", "field", "wood", "hill"};

    FString Prefix = GetRandomBool(0.5f) ? GetRandomElement(Prefixes) + " " : "";
    FString Suffix = GetRandomBool(0.7f) ? GetRandomElement(Suffixes) : "";

    return FString::Printf(TEXT("%s%s %s"), *Prefix, *Type, *Suffix);
}

FVector UProceduralContentGenerator::GenerateLocationPosition(const FVector& BasePosition, float MinDistance, float MaxDistance)
{
    float Distance = GetRandomFloat(MinDistance, MaxDistance);
    float Angle = GetRandomFloat(0.0f, 2.0f * PI);

    FVector Offset;
    Offset.X = FMath::Cos(Angle) * Distance;
    Offset.Y = FMath::Sin(Angle) * Distance;
    Offset.Z = 0.0f;

    return BasePosition + Offset;
}

TArray<FString> UProceduralContentGenerator::GenerateLocationFeatures(const FString& Type, int32 FeatureCount)
{
    TArray<FString> Features;

    for (int32 i = 0; i < FeatureCount; ++i)
    {
        FString Feature = GetRandomElement(LocationFeatures);
        if (!Features.Contains(Feature))
        {
            Features.Add(Feature);
        }
    }

    return Features;
}

int32 UProceduralContentGenerator::CalculateLocationDanger(const FString& Type, const TArray<FString>& Features)
{
    int32 Danger = 1;

    if (Type.Contains("Cave") || Type.Contains("Ruins"))
    {
        Danger += 2;
    }
    else if (Type.Contains("Forest") || Type.Contains("Mine"))
    {
        Danger += 1;
    }

    for (const FString& Feature : Features)
    {
        if (Feature.Contains("Mine") || Feature.Contains("Tower"))
        {
            Danger += 1;
        }
    }

    return FMath::Clamp(Danger, 1, 5);
}

int32 UProceduralContentGenerator::CalculateLocationValue(const FString& Type, const TArray<FString>& Features)
{
    int32 Value = 1;

    if (Type.Contains("Castle") || Type.Contains("Town"))
    {
        Value += 3;
    }
    else if (Type.Contains("Village") || Type.Contains("Farmstead"))
    {
        Value += 2;
    }

    for (const FString& Feature : Features)
    {
        if (Feature.Contains("Market") || Feature.Contains("Mine"))
        {
            Value += 2;
        }
        else if (Feature.Contains("Tavern") || Feature.Contains("Temple"))
        {
            Value += 1;
        }
    }

    return FMath::Clamp(Value, 1, 5);
}

FString UProceduralContentGenerator::GenerateQuestTitle(const FString& Type, int32 Difficulty)
{
    TArray<FString> Titles;

    if (Type == "Delivery")
    {
        Titles = {"Deliver the Package", "Urgent Delivery", "Special Courier", "Important Message"};
    }
    else if (Type == "Combat")
    {
        Titles = {"Clear the Threat", "Defend the Area", "Hunt the Beast", "Eliminate the Danger"};
    }
    else if (Type == "Exploration")
    {
        Titles = {"Explore the Unknown", "Map the Territory", "Discover the Secret", "Chart the Lands"};
    }
    else if (Type == "Social")
    {
        Titles = {"Make an Alliance", "Resolve the Dispute", "Help the Needy", "Build Relationships"};
    }

    if (Titles.Num() > 0)
    {
        return GetRandomElement(Titles);
    }

    return "Generic Quest";
}

FString UProceduralContentGenerator::GenerateQuestDescription(const FProceduralQuest& Quest)
{
    return FString::Printf(TEXT("A %s quest of difficulty %d. Complete the objectives to earn %d gold."),
        *Quest.QuestType, Quest.Difficulty, Quest.RewardGold);
}

TArray<FString> UProceduralContentGenerator::GenerateQuestObjectives(const FString& Type, int32 Difficulty)
{
    TArray<FString> Objectives;

    int32 ObjectiveCount = FMath::Clamp(Difficulty, 1, 3);

    for (int32 i = 0; i < ObjectiveCount; ++i)
    {
        if (Type == "Delivery")
        {
            Objectives.Add("Deliver the item to the destination");
        }
        else if (Type == "Combat")
        {
            Objectives.Add(FString::Printf(TEXT("Defeat %d enemies"), Difficulty * 2));
        }
        else if (Type == "Exploration")
        {
            Objectives.Add("Explore the designated area");
        }
        else if (Type == "Social")
        {
            Objectives.Add("Complete the social interaction");
        }
    }

    return Objectives;
}

int32 UProceduralContentGenerator::CalculateQuestReward(const FProceduralQuest& Quest)
{
    return Quest.Difficulty * 25 + GetRandomInt(0, 50);
}

FString UProceduralContentGenerator::GenerateNPCName(const FString& Type)
{
    return GetRandomElement(NPCNames);
}

TArray<FString> UProceduralContentGenerator::GenerateNPCBackground(const FString& Type, int32 Count)
{
    TArray<FString> Background;

    for (int32 i = 0; i < Count; ++i)
    {
        Background.Add(GetRandomElement(NPCBackgrounds));
    }

    return Background;
}

TMap<FString, int32> UProceduralContentGenerator::GenerateNPCSkills(const FString& Type, int32 PowerLevel)
{
    TMap<FString, int32> Skills;

    if (Type == "Warrior" || Type == "Knight" || Type == "Guard")
    {
        Skills.Add("Combat", PowerLevel * 3);
        Skills.Add("Defense", PowerLevel * 2);
    }
    else if (Type == "Merchant")
    {
        Skills.Add("Trading", PowerLevel * 3);
        Skills.Add("Persuasion", PowerLevel * 2);
    }
    else if (Type == "Farmer")
    {
        Skills.Add("Farming", PowerLevel * 2);
        Skills.Add("Animal Care", PowerLevel * 2);
    }

    return Skills;
}

TMap<FString, FString> UProceduralContentGenerator::GenerateNPCRelationships(const FProceduralNPC& NPC)
{
    TMap<FString, FString> Relationships;

    // Generate some random relationships
    TArray<FString> RelationTypes = {"Friend", "Rival", "Family", "Colleague", "Acquaintance"};

    int32 RelationCount = GetRandomInt(1, 3);
    for (int32 i = 0; i < RelationCount; ++i)
    {
        FString RelationType = GetRandomElement(RelationTypes);
        FString RelatedNPC = GenerateNPCName("Generic");
        Relationships.Add(RelatedNPC, RelationType);
    }

    return Relationships;
}

// Utility functions
FString UProceduralContentGenerator::GetRandomElement(const TArray<FString>& Array)
{
    if (Array.Num() == 0) return "";
    return Array[RandomStream.RandRange(0, Array.Num() - 1)];
}

int32 UProceduralContentGenerator::GetRandomInt(int32 Min, int32 Max)
{
    return RandomStream.RandRange(Min, Max);
}

float UProceduralContentGenerator::GetRandomFloat(float Min, float Max)
{
    return RandomStream.FRandRange(Min, Max);
}

bool UProceduralContentGenerator::GetRandomBool(float TrueChance)
{
    return RandomStream.FRand() < TrueChance;
}