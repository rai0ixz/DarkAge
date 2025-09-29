#include "Core/AdvancedQuestGenerationSubsystem.h"
#include "Core/NPCEcosystemSubsystem.h"
#include "Core/FactionManagerSubsystem.h"
#include "Core/EconomySubsystem.h"
#include "Core/WorldPopulationSubsystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UAdvancedQuestGenerationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    InitializeQuestTemplates();
    InitializeQuestParameters();
    
    QuestGenerationTimer = 0.0f;
    NextQuestID = 1;
    
    UE_LOG(LogTemp, Log, TEXT("AdvancedQuestGenerationSubsystem initialized with %d quest templates"), QuestTemplates.Num());
}

void UAdvancedQuestGenerationSubsystem::Tick(float DeltaTime)
{
    
    QuestGenerationTimer += DeltaTime;
    
    // Generate new quests every 60 seconds
    if (QuestGenerationTimer >= 60.0f)
    {
        GenerateContextualQuests();
        QuestGenerationTimer = 0.0f;
    }
    
    // Update active quests
    UpdateActiveQuests(DeltaTime);
    
    // Clean up expired quests
    CleanupExpiredQuests();
}

void UAdvancedQuestGenerationSubsystem::InitializeQuestTemplates()
{
    // Survival Quests
    FQuestTemplate SurvivalTemplate;
    SurvivalTemplate.QuestType = EQuestCategory::Survival;
    SurvivalTemplate.Title = TEXT("Gather Resources");
    SurvivalTemplate.Description = TEXT("Collect {ResourceType} to survive the harsh conditions");
    SurvivalTemplate.BaseReward = 50.0f;
    SurvivalTemplate.BaseDifficulty = 0.3f;
    SurvivalTemplate.RequiredLevel = 1;
    SurvivalTemplate.TimeLimit = 1800.0f; // 30 minutes
    QuestTemplates.Add(TEXT("GatherResources"), SurvivalTemplate);
    
    // Combat Quests
    FQuestTemplate CombatTemplate;
    CombatTemplate.QuestType = EQuestCategory::Combat;
    CombatTemplate.Title = TEXT("Eliminate Threat");
    CombatTemplate.Description = TEXT("Eliminate {EnemyCount} {EnemyType} threatening {Location}");
    CombatTemplate.BaseReward = 100.0f;
    CombatTemplate.BaseDifficulty = 0.6f;
    CombatTemplate.RequiredLevel = 3;
    CombatTemplate.TimeLimit = 3600.0f; // 1 hour
    QuestTemplates.Add(TEXT("EliminateThreat"), CombatTemplate);
    
    // Trade Quests
    FQuestTemplate TradeTemplate;
    TradeTemplate.QuestType = EQuestCategory::Trade;
    TradeTemplate.Title = TEXT("Merchant's Request");
    TradeTemplate.Description = TEXT("Deliver {ItemCount} {ItemType} to {Destination}");
    TradeTemplate.BaseReward = 75.0f;
    TradeTemplate.BaseDifficulty = 0.4f;
    TradeTemplate.RequiredLevel = 2;
    TradeTemplate.TimeLimit = 2400.0f; // 40 minutes
    QuestTemplates.Add(TEXT("MerchantRequest"), TradeTemplate);
    
    // Social Quests
    FQuestTemplate SocialTemplate;
    SocialTemplate.QuestType = EQuestCategory::Social;
    SocialTemplate.Title = TEXT("Diplomatic Mission");
    SocialTemplate.Description = TEXT("Negotiate with {FactionName} to {Objective}");
    SocialTemplate.BaseReward = 120.0f;
    SocialTemplate.BaseDifficulty = 0.5f;
    SocialTemplate.RequiredLevel = 4;
    SocialTemplate.TimeLimit = 1800.0f; // 30 minutes
    QuestTemplates.Add(TEXT("DiplomaticMission"), SocialTemplate);
    
    // Investigation Quests
    FQuestTemplate InvestigationTemplate;
    InvestigationTemplate.QuestType = EQuestCategory::Investigation;
    InvestigationTemplate.Title = TEXT("Mystery Investigation");
    InvestigationTemplate.Description = TEXT("Investigate the {Mystery} in {Location}");
    InvestigationTemplate.BaseReward = 90.0f;
    InvestigationTemplate.BaseDifficulty = 0.7f;
    InvestigationTemplate.RequiredLevel = 5;
    InvestigationTemplate.TimeLimit = 2700.0f; // 45 minutes
    QuestTemplates.Add(TEXT("Investigation"), InvestigationTemplate);
    
    // Crafting Quests
    FQuestTemplate CraftingTemplate;
    CraftingTemplate.QuestType = EQuestCategory::Crafting;
    CraftingTemplate.Title = TEXT("Master Craftsman");
    CraftingTemplate.Description = TEXT("Craft {ItemCount} {ItemType} of {Quality} quality");
    CraftingTemplate.BaseReward = 80.0f;
    CraftingTemplate.BaseDifficulty = 0.5f;
    CraftingTemplate.RequiredLevel = 3;
    CraftingTemplate.TimeLimit = 3600.0f; // 1 hour
    QuestTemplates.Add(TEXT("MasterCraftsman"), CraftingTemplate);
    
    UE_LOG(LogTemp, Log, TEXT("Initialized %d quest templates"), QuestTemplates.Num());
}

void UAdvancedQuestGenerationSubsystem::InitializeQuestParameters()
{
    // Resource types for survival quests
    ResourceTypes.Add(TEXT("Wood"));
    ResourceTypes.Add(TEXT("Stone"));
    ResourceTypes.Add(TEXT("Iron"));
    ResourceTypes.Add(TEXT("Food"));
    ResourceTypes.Add(TEXT("Medicine"));
    
    // Enemy types for combat quests
    EnemyTypes.Add(TEXT("Wolves"));
    EnemyTypes.Add(TEXT("Bandits"));
    EnemyTypes.Add(TEXT("Undead"));
    EnemyTypes.Add(TEXT("Goblins"));
    EnemyTypes.Add(TEXT("Bears"));
    
    // Item types for trade/crafting quests
    ItemTypes.Add(TEXT("Weapons"));
    ItemTypes.Add(TEXT("Armor"));
    ItemTypes.Add(TEXT("Tools"));
    ItemTypes.Add(TEXT("Potions"));
    ItemTypes.Add(TEXT("Jewelry"));
    
    // Locations
    Locations.Add(TEXT("Millhaven"));
    Locations.Add(TEXT("Dark Forest"));
    Locations.Add(TEXT("Iron Mines"));
    Locations.Add(TEXT("Ancient Ruins"));
    Locations.Add(TEXT("Riverside"));
    
    // Mysteries for investigation quests
    Mysteries.Add(TEXT("disappearances"));
    Mysteries.Add(TEXT("strange lights"));
    Mysteries.Add(TEXT("cursed artifacts"));
    Mysteries.Add(TEXT("haunted grounds"));
    Mysteries.Add(TEXT("missing caravan"));
    
    // Quality levels
    QualityLevels.Add(TEXT("Common"));
    QualityLevels.Add(TEXT("Uncommon"));
    QualityLevels.Add(TEXT("Rare"));
    QualityLevels.Add(TEXT("Epic"));
    
    UE_LOG(LogTemp, Log, TEXT("Initialized quest parameters"));
}

void UAdvancedQuestGenerationSubsystem::GenerateContextualQuests()
{
}

FGameStateContext UAdvancedQuestGenerationSubsystem::AnalyzeGameState()
{
    FGameStateContext Context;
    
    // Analyze player needs (would integrate with survival system)
    Context.PlayerNeedsFood = FMath::FRand() < 0.4f; // Simplified
    Context.PlayerNeedsMedicine = FMath::FRand() < 0.2f;
    
    // Analyze threat levels (would integrate with NPC ecosystem)
    Context.EnemyThreatLevel = FMath::FRandRange(0.0f, 1.0f);
    if (EnemyTypes.Num() > 0)
        Context.PrimaryThreatType = EnemyTypes[FMath::RandRange(0, EnemyTypes.Num() - 1)];
    else
        Context.PrimaryThreatType = TEXT("Unknown");
    if (Locations.Num() > 0)
        Context.ThreatLocation = Locations[FMath::RandRange(0, Locations.Num() - 1)];
    else
        Context.ThreatLocation = TEXT("Unknown");
    
    // Analyze economic state (would integrate with economy system)
    Context.EconomicInstability = FMath::FRandRange(0.0f, 1.0f);
    if (ResourceTypes.Num() > 0)
        Context.NeededResource = ResourceTypes[FMath::RandRange(0, ResourceTypes.Num() - 1)];
    else
        Context.NeededResource = TEXT("Unknown");
    
    // Analyze political state (would integrate with faction system)
    Context.PoliticalTension = FMath::FRandRange(0.0f, 1.0f);
    if (Context.PoliticalTension > 0.5f)
    {
        Context.ConflictingFactions.Add(TEXT("Iron Brotherhood"));
        Context.ConflictingFactions.Add(TEXT("Merchant Coalition"));
    }
    
    return Context;
}

void UAdvancedQuestGenerationSubsystem::GenerateSurvivalQuest(const FString& ResourceType)
{
    if (FQuestTemplate* Template = QuestTemplates.Find(TEXT("GatherResources")))
    {
        FDynamicQuest NewQuest = CreateQuestFromTemplate(*Template);

        // Reference actual food source location from WorldPopulationSubsystem
        UWorldPopulationSubsystem* WorldPopSubsystem = GetGameInstance()->GetSubsystem<UWorldPopulationSubsystem>();
        FVector FoodLocation = FVector::ZeroVector;
        if (WorldPopSubsystem)
        {
            TArray<FWorldObjectInfo> FoodSources = WorldPopSubsystem->GetWorldObjectsByType(TEXT("Food"));
            if (FoodSources.Num() > 0)
            {
                FoodLocation = FoodSources[0].Location;
                NewQuest.TargetLocation = FoodLocation;
            }
        }

        // Customize for specific resource
        NewQuest.QuestName = FString::Printf(TEXT("Gather %s"), *ResourceType);
        NewQuest.Description = FString::Printf(TEXT("Collect %d %s to meet urgent survival needs at location (%s)"),
            FMath::RandRange(5, 15), *ResourceType, *FoodLocation.ToString());

        // Set objectives
        FQuestObjective Objective;
        Objective.ObjectiveType = EObjectiveType::Collect;
        Objective.TargetItem = ResourceType;
        Objective.RequiredCount = FMath::RandRange(5, 15);
        Objective.CurrentCount = 0;
        Objective.bIsCompleted = false;
        Objective.TargetLocation = FoodLocation.ToString();
        NewQuest.Objectives.Add(Objective);

        // Adjust reward based on resource rarity
        if (ResourceType == TEXT("Iron") || ResourceType == TEXT("Medicine"))
        {
            if (NewQuest.Rewards.Contains(TEXT("Gold")))
            {
                NewQuest.Rewards[TEXT("Gold")] *= 1.5f;
            }
        }

        AddQuestToPool(NewQuest);
        UE_LOG(LogTemp, Log, TEXT("Generated survival quest: %s at location %s"), *NewQuest.QuestName, *FoodLocation.ToString());
    }
}

void UAdvancedQuestGenerationSubsystem::GenerateCombatQuest(const FString& EnemyType, const FString& Location)
{
    if (FQuestTemplate* Template = QuestTemplates.Find(TEXT("EliminateThreat")))
    {
        FDynamicQuest NewQuest = CreateQuestFromTemplate(*Template);
        
        int32 EnemyCount = FMath::RandRange(3, 8);
        
        NewQuest.QuestName = FString::Printf(TEXT("Clear %s"), *Location);
        NewQuest.Description = FString::Printf(TEXT("Eliminate %d %s threatening %s"), 
            EnemyCount, *EnemyType, *Location);
        
        // Set objectives
        FQuestObjective Objective;
        Objective.ObjectiveType = EObjectiveType::Kill;
        Objective.TargetEnemy = EnemyType;
        Objective.RequiredCount = EnemyCount;
        Objective.CurrentCount = 0;
        Objective.bIsCompleted = false;
        NewQuest.Objectives.Add(Objective);
        
        // Adjust difficulty and reward based on enemy type
        if (EnemyType == TEXT("Undead") || EnemyType == TEXT("Bears"))
        {
            NewQuest.Difficulty = EDynamicQuestDifficulty::Hard;
            if (NewQuest.Rewards.Contains(TEXT("Gold")))
            {
                NewQuest.Rewards[TEXT("Gold")] *= 1.4f;
            }
        }
        
        AddQuestToPool(NewQuest);
        
        UE_LOG(LogTemp, Log, TEXT("Generated combat quest: %s"), *NewQuest.QuestName);
    }
}

void UAdvancedQuestGenerationSubsystem::GenerateTradeQuest(const FString& NeededResource)
{
    if (FQuestTemplate* Template = QuestTemplates.Find(TEXT("MerchantRequest")))
    {
        FDynamicQuest NewQuest = CreateQuestFromTemplate(*Template);
        
        FString Origin = Locations[FMath::RandRange(0, Locations.Num() - 1)];
        FString Destination;
        do {
            Destination = Locations[FMath::RandRange(0, Locations.Num() - 1)];
        } while (Destination == Origin);
        
        int32 ItemCount = FMath::RandRange(2, 6);
        
        NewQuest.QuestName = FString::Printf(TEXT("Trade Route: %s to %s"), *Origin, *Destination);
        NewQuest.Description = FString::Printf(TEXT("Transport %d %s from %s to %s"), 
            ItemCount, *NeededResource, *Origin, *Destination);
        
        // Set objectives
        FQuestObjective CollectObjective;
        CollectObjective.ObjectiveType = EObjectiveType::Collect;
        CollectObjective.TargetItem = NeededResource;
        CollectObjective.RequiredCount = ItemCount;
        CollectObjective.CurrentCount = 0;
        CollectObjective.bIsCompleted = false;
        NewQuest.Objectives.Add(CollectObjective);
        
        FQuestObjective DeliverObjective;
        DeliverObjective.ObjectiveType = EObjectiveType::Deliver;
        DeliverObjective.TargetLocation = Destination;
        DeliverObjective.RequiredCount = ItemCount;
        DeliverObjective.CurrentCount = 0;
        DeliverObjective.bIsCompleted = false;
        NewQuest.Objectives.Add(DeliverObjective);
        
        AddQuestToPool(NewQuest);
        
        UE_LOG(LogTemp, Log, TEXT("Generated trade quest: %s"), *NewQuest.QuestName);
    }
}

void UAdvancedQuestGenerationSubsystem::GenerateDiplomaticQuest(const FString& Faction1, const FString& Faction2)
{
    if (FQuestTemplate* Template = QuestTemplates.Find(TEXT("DiplomaticMission")))
    {
        FDynamicQuest NewQuest = CreateQuestFromTemplate(*Template);
        
        TArray<FString> Objectives;
        Objectives.Add(TEXT("negotiate peace"));
        Objectives.Add(TEXT("establish trade agreement"));
        Objectives.Add(TEXT("resolve border dispute"));
        
        FString Objective = Objectives[FMath::RandRange(0, Objectives.Num() - 1)];
        
        NewQuest.QuestName = FString::Printf(TEXT("Diplomatic Crisis"));
        NewQuest.Description = FString::Printf(TEXT("Help %s and %s %s"), 
            *Faction1, *Faction2, *Objective);
        
        // Set objectives
        FQuestObjective TalkObjective;
        TalkObjective.ObjectiveType = EObjectiveType::TalkTo;
        TalkObjective.TargetNPC = FString::Printf(TEXT("%s Representative"), *Faction1);
        TalkObjective.RequiredCount = 1;
        TalkObjective.CurrentCount = 0;
        TalkObjective.bIsCompleted = false;
        NewQuest.Objectives.Add(TalkObjective);
        
        FQuestObjective TalkObjective2;
        TalkObjective2.ObjectiveType = EObjectiveType::TalkTo;
        TalkObjective2.TargetNPC = FString::Printf(TEXT("%s Representative"), *Faction2);
        TalkObjective2.RequiredCount = 1;
        TalkObjective2.CurrentCount = 0;
        TalkObjective2.bIsCompleted = false;
        NewQuest.Objectives.Add(TalkObjective2);
        
        // Higher reward for diplomatic success
        if (NewQuest.Rewards.Contains(TEXT("Gold")))
        {
            NewQuest.Rewards[TEXT("Gold")] *= 1.6f;
        }
        
        AddQuestToPool(NewQuest);
        
        UE_LOG(LogTemp, Log, TEXT("Generated diplomatic quest: %s"), *NewQuest.QuestName);
    }
}

void UAdvancedQuestGenerationSubsystem::GenerateRandomQuest()
{
    TArray<FString> TemplateKeys;
    QuestTemplates.GetKeys(TemplateKeys);
    
    if (TemplateKeys.Num() == 0)
        return;
    
    FString RandomTemplateKey = TemplateKeys[FMath::RandRange(0, TemplateKeys.Num() - 1)];
    FQuestTemplate* Template = QuestTemplates.Find(RandomTemplateKey);
    
    if (!Template)
        return;
    
    FDynamicQuest NewQuest = CreateQuestFromTemplate(*Template);
    
    // Customize based on template type
    if (Template->QuestType == EQuestCategory::Investigation)
    {
        FString Mystery = Mysteries[FMath::RandRange(0, Mysteries.Num() - 1)];
        FString Location = Locations[FMath::RandRange(0, Locations.Num() - 1)];
        
        NewQuest.QuestName = FString::Printf(TEXT("Investigate %s"), *Mystery);
        NewQuest.Description = FString::Printf(TEXT("Investigate the %s in %s"), *Mystery, *Location);
        
        FQuestObjective Objective;
        Objective.ObjectiveType = EObjectiveType::Investigate;
        Objective.TargetLocation = Location;
        Objective.RequiredCount = 1;
        Objective.CurrentCount = 0;
        Objective.bIsCompleted = false;
        NewQuest.Objectives.Add(Objective);
    }
    else if (Template->QuestType == EQuestCategory::Crafting)
    {
        FString ItemType = ItemTypes[FMath::RandRange(0, ItemTypes.Num() - 1)];
        FString Quality = QualityLevels[FMath::RandRange(0, QualityLevels.Num() - 1)];
        int32 ItemCount = FMath::RandRange(1, 3);
        
        NewQuest.QuestName = FString::Printf(TEXT("Craft %s %s"), *Quality, *ItemType);
        NewQuest.Description = FString::Printf(TEXT("Craft %d %s %s"), ItemCount, *Quality, *ItemType);
        
        FQuestObjective Objective;
        Objective.ObjectiveType = EObjectiveType::Craft;
        Objective.TargetItem = FString::Printf(TEXT("%s %s"), *Quality, *ItemType);
        Objective.RequiredCount = ItemCount;
        Objective.CurrentCount = 0;
        Objective.bIsCompleted = false;
        NewQuest.Objectives.Add(Objective);
        
        // Higher quality items give more reward
        if (Quality == TEXT("Rare"))
        {
            if (NewQuest.Rewards.Contains(TEXT("Gold")))
            {
                NewQuest.Rewards[TEXT("Gold")] *= 1.5f;
            }
        }
        else if (Quality == TEXT("Epic"))
        {
            if (NewQuest.Rewards.Contains(TEXT("Gold")))
            {
                NewQuest.Rewards[TEXT("Gold")] *= 2.0f;
            }
        }
    }
    
    AddQuestToPool(NewQuest);
    
    UE_LOG(LogTemp, Log, TEXT("Generated random quest: %s"), *NewQuest.QuestName);
}

FDynamicQuest UAdvancedQuestGenerationSubsystem::CreateQuestFromTemplate(const FQuestTemplate& Template)
{
    FDynamicQuest NewQuest;
    NewQuest.QuestID = FString::FromInt(NextQuestID++);
    NewQuest.QuestName = Template.Title;
    NewQuest.Description = Template.Description;
    
    // Convert EQuestCategory to EDynamicQuestType
    switch (Template.QuestType)
    {
        case EQuestCategory::Survival:
            NewQuest.QuestType = EDynamicQuestType::Gathering;
            break;
        case EQuestCategory::Combat:
            NewQuest.QuestType = EDynamicQuestType::Elimination;
            break;
        case EQuestCategory::Trade:
            NewQuest.QuestType = EDynamicQuestType::Delivery;
            break;
        case EQuestCategory::Social:
            NewQuest.QuestType = EDynamicQuestType::Social;
            break;
        case EQuestCategory::Investigation:
            NewQuest.QuestType = EDynamicQuestType::Investigation;
            break;
        case EQuestCategory::Crafting:
            NewQuest.QuestType = EDynamicQuestType::Gathering;
            break;
        default:
            NewQuest.QuestType = EDynamicQuestType::Delivery;
            break;
    }
    
    // Set difficulty based on template difficulty
    if (Template.BaseDifficulty < 0.3f)
        NewQuest.Difficulty = EDynamicQuestDifficulty::Easy;
    else if (Template.BaseDifficulty < 0.6f)
        NewQuest.Difficulty = EDynamicQuestDifficulty::Medium;
    else if (Template.BaseDifficulty < 0.8f)
        NewQuest.Difficulty = EDynamicQuestDifficulty::Hard;
    else
        NewQuest.Difficulty = EDynamicQuestDifficulty::Extreme;
    
    NewQuest.Status = EDynamicQuestStatus::Generated;
    NewQuest.TimeLimit = Template.TimeLimit;
    NewQuest.RemainingTime = Template.TimeLimit;
    
    // Set rewards
    float GoldReward = Template.BaseReward * FMath::FRandRange(0.8f, 1.2f);
    NewQuest.Rewards.Add(TEXT("Gold"), GoldReward);
    NewQuest.Rewards.Add(TEXT("Experience"), GoldReward * 2.0f);
    
    return NewQuest;
}

void UAdvancedQuestGenerationSubsystem::AddQuestToPool(const FDynamicQuest& Quest)
{
    AvailableQuests.Add(Quest);
    
    // Limit the number of available quests
    if (AvailableQuests.Num() > MaxAvailableQuests)

    {
        // Remove oldest quest
        AvailableQuests.RemoveAt(0);
    }
    
    OnQuestGenerated.Broadcast(Quest);
}

void UAdvancedQuestGenerationSubsystem::UpdateActiveQuests(float DeltaTime)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    float CurrentTime = World->GetTimeSeconds();
    
    for (FDynamicQuest& Quest : ActiveQuests)
    {
        if (Quest.bIsCompleted || Quest.bIsExpired)
            continue;
        
        // Check for time expiration
        if (Quest.TimeLimit > 0.0f && (CurrentTime - Quest.CreationTime) > Quest.TimeLimit)
        {
            Quest.bIsExpired = true;
            UE_LOG(LogTemp, Warning, TEXT("Quest expired: %s"), *Quest.Title);
            OnQuestExpired.Broadcast(Quest);
            continue;
        }
        
        // Update quest objectives (simplified - would integrate with game systems)
        UpdateQuestObjectives(Quest);
        
        // Check if quest is completed
        bool bAllObjectivesComplete = true;
        for (const FQuestObjective& Objective : Quest.Objectives)
        {
            if (!Objective.bIsCompleted)
            {
                bAllObjectivesComplete = false;
                break;
            }
        }
        
        if (bAllObjectivesComplete && !Quest.bIsCompleted)
        {
            CompleteQuest(Quest);
        }
    }
}

void UAdvancedQuestGenerationSubsystem::UpdateQuestObjectives(FDynamicQuest& Quest)
{
    // This would integrate with various game systems to track objective progress
    // For now, we'll simulate some progress
    
    for (FQuestObjective& Objective : Quest.Objectives)
    {
        if (Objective.bIsCompleted)
            continue;
        
        // Simulate random progress (in real implementation, this would be driven by player actions)
        if (FMath::FRand() < 0.01f) // 1% chance per update
        {
            Objective.CurrentCount = FMath::Min(Objective.CurrentCount + 1, Objective.RequiredCount);
            
            if (Objective.CurrentCount >= Objective.RequiredCount)
            {
                Objective.bIsCompleted = true;
                UE_LOG(LogTemp, Log, TEXT("Quest objective completed: %s"), *Quest.Title);
                OnQuestObjectiveCompleted.Broadcast(Quest, Objective);
            }
        }
    }
}

void UAdvancedQuestGenerationSubsystem::CompleteQuest(FDynamicQuest& Quest)
{
    Quest.bIsCompleted = true;
    
    UE_LOG(LogTemp, Log, TEXT("Quest completed: %s (Reward: %f gold, %f XP)"), 
        *Quest.Title, Quest.GoldReward, Quest.ExperienceReward);
    
    // Award rewards (would integrate with player progression system)
    OnQuestCompleted.Broadcast(FName(*Quest.QuestID));
}

void UAdvancedQuestGenerationSubsystem::CleanupExpiredQuests()
{
    // Remove expired quests from available quests
    AvailableQuests.RemoveAll([](const FDynamicQuest& Quest)
    {
        return Quest.bIsExpired;
    });
    
    // Remove completed/expired quests from active quests after some time
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    float CurrentTime = World->GetTimeSeconds();
    ActiveQuests.RemoveAll([CurrentTime](const FDynamicQuest& Quest)
    {
        return (Quest.bIsCompleted || Quest.bIsExpired) && 
               (CurrentTime - Quest.CreationTime) > 300.0f; // Keep for 5 minutes after completion
    });
}

// Public Interface Functions
TArray<FDynamicQuest> UAdvancedQuestGenerationSubsystem::GetAvailableQuests() const
{
    return AvailableQuests;
}

TArray<FDynamicQuest> UAdvancedQuestGenerationSubsystem::GetActiveQuests() const
{
    return ActiveQuests;
}

bool UAdvancedQuestGenerationSubsystem::AcceptQuest(int32 QuestID)
{
    // Find quest in available quests
    int32 QuestIndex = AvailableQuests.IndexOfByPredicate([QuestID](const FDynamicQuest& Quest)
    {
        return Quest.QuestID == FString::FromInt(QuestID);
    });
    
    if (QuestIndex == INDEX_NONE)
    {
        UE_LOG(LogTemp, Warning, TEXT("AcceptQuest: Quest ID %d not found in available quests"), QuestID);
        return false;
    }
    
    // Move quest to active quests
    FDynamicQuest Quest = AvailableQuests[QuestIndex];
    Quest.Status = EDynamicQuestStatus::Active;
    ActiveQuests.Add(Quest);
    AvailableQuests.RemoveAt(QuestIndex);
    
    UE_LOG(LogTemp, Log, TEXT("Quest accepted: %s"), *Quest.QuestName);
    OnQuestAccepted.Broadcast(Quest);
    
    return true;
}

bool UAdvancedQuestGenerationSubsystem::AbandonQuest(int32 QuestID)
{
    // Find quest in active quests
    int32 QuestIndex = ActiveQuests.IndexOfByPredicate([QuestID](const FDynamicQuest& Quest)
    {
        return Quest.QuestID == FString::FromInt(QuestID);
    });
    
    if (QuestIndex == INDEX_NONE)
    {
        UE_LOG(LogTemp, Warning, TEXT("AbandonQuest: Quest ID %d not found in active quests"), QuestID);
        return false;
    }
    
    FDynamicQuest Quest = ActiveQuests[QuestIndex];
    ActiveQuests.RemoveAt(QuestIndex);
    
    UE_LOG(LogTemp, Log, TEXT("Quest abandoned: %s"), *Quest.QuestName);
    OnQuestAbandoned.Broadcast(FName(*Quest.QuestID));
    
    return true;
}

void UAdvancedQuestGenerationSubsystem::ForceGenerateQuest(EQuestCategory QuestType)
{
    FString TemplateKey;
    
    switch (QuestType)
    {
    case EQuestCategory::Survival:
        TemplateKey = TEXT("GatherResources");
        break;
    case EQuestCategory::Combat:
        TemplateKey = TEXT("EliminateThreat");
        break;
    case EQuestCategory::Trade:
        TemplateKey = TEXT("MerchantRequest");
        break;
    case EQuestCategory::Social:
        TemplateKey = TEXT("DiplomaticMission");
        break;
    case EQuestCategory::Investigation:
        TemplateKey = TEXT("Investigation");
        break;
    case EQuestCategory::Crafting:
        TemplateKey = TEXT("MasterCraftsman");
        break;
    default:
        GenerateRandomQuest();
        return;
    }
    
    if (FQuestTemplate* Template = QuestTemplates.Find(TemplateKey))
    {
        FDynamicQuest NewQuest = CreateQuestFromTemplate(*Template);
        
        // Apply basic customization
        if (QuestType == EQuestCategory::Survival)
        {
            GenerateSurvivalQuest(ResourceTypes[FMath::RandRange(0, ResourceTypes.Num() - 1)]);
        }
        else if (QuestType == EQuestCategory::Combat)
        {
            GenerateCombatQuest(EnemyTypes[FMath::RandRange(0, EnemyTypes.Num() - 1)],
                              Locations[FMath::RandRange(0, Locations.Num() - 1)]);
        }
        else
        {
            AddQuestToPool(NewQuest);
        }
    }
}

int32 UAdvancedQuestGenerationSubsystem::GetAvailableQuestCount() const
{
    return AvailableQuests.Num();
}

int32 UAdvancedQuestGenerationSubsystem::GetActiveQuestCount() const
{
    return ActiveQuests.Num();
}