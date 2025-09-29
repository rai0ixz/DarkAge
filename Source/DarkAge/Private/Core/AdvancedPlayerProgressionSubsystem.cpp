#include "Core/AdvancedPlayerProgressionSubsystem.h"
#include "Components/StatlineComponent.h"
#include "Components/PlayerSkillsComponent.h"
#include "Components/FactionReputationComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

void UAdvancedPlayerProgressionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    
    InitializeProgressionTrees();
    InitializeAchievementSystem();
    InitializeLegacySystem();
    
    PlayerLevel = 1;
    TotalExperience = 0.0f;
    AvailableAttributePoints = 0;
    AvailableSkillPoints = 0;
    
    ProgressionUpdateTimer = 0.0f;
    
    UE_LOG(LogTemp, Log, TEXT("AdvancedPlayerProgressionSubsystem initialized"));
}

void UAdvancedPlayerProgressionSubsystem::Tick(float DeltaTime)
{
    // UGameInstanceSubsystem doesn't have Tick method, remove this call
    
    ProgressionUpdateTimer += DeltaTime;
    
    // Update progression tracking every 5 seconds
    if (ProgressionUpdateTimer >= 5.0f)
    {
        UpdateProgressionTracking();
        CheckAchievements();
        UpdateLegacyEffects();
        ProgressionUpdateTimer = 0.0f;
    }
    
    // Update active progression effects
    UpdateActiveEffects(DeltaTime);
}

void UAdvancedPlayerProgressionSubsystem::InitializeProgressionTrees()
{
    // Combat Progression Tree
    FProgressionTree CombatTree;
    CombatTree.TreeName = TEXT("Combat Mastery");
    CombatTree.Description = TEXT("Master the arts of war and combat");
    CombatTree.TreeType = EProgressionTreeType::Combat;
    
    // Combat nodes
    FProgressionNode WeaponMastery;
    WeaponMastery.NodeID = TEXT("WeaponMastery");
    WeaponMastery.NodeName = TEXT("Weapon Mastery");
    WeaponMastery.Description = TEXT("Increases damage with all weapons by 15%");
    WeaponMastery.RequiredLevel = 5;
    WeaponMastery.SkillPointCost = 3;
    WeaponMastery.Prerequisites.Empty();
    WeaponMastery.bIsUnlocked = false;
    CombatTree.Nodes.Add(TEXT("WeaponMastery"), WeaponMastery);
    
    FProgressionNode CombatReflexes;
    CombatReflexes.NodeID = TEXT("CombatReflexes");
    CombatReflexes.NodeName = TEXT("Combat Reflexes");
    CombatReflexes.Description = TEXT("Increases dodge chance by 20%");
    CombatReflexes.RequiredLevel = 8;
    CombatReflexes.SkillPointCost = 4;
    CombatReflexes.Prerequisites.Add(TEXT("WeaponMastery"));
    CombatReflexes.bIsUnlocked = false;
    CombatTree.Nodes.Add(TEXT("CombatReflexes"), CombatReflexes);
    
    FProgressionNode BattleFrenzy;
    BattleFrenzy.NodeID = TEXT("BattleFrenzy");
    BattleFrenzy.NodeName = TEXT("Battle Frenzy");
    BattleFrenzy.Description = TEXT("Each kill increases damage by 10% for 30 seconds (stacks up to 5 times)");
    BattleFrenzy.RequiredLevel = 12;
    BattleFrenzy.SkillPointCost = 5;
    BattleFrenzy.Prerequisites.Add(TEXT("CombatReflexes"));
    BattleFrenzy.bIsUnlocked = false;
    CombatTree.Nodes.Add(TEXT("BattleFrenzy"), BattleFrenzy);
    
    ProgressionTrees.Add(TEXT("Combat"), CombatTree);
    
    // Survival Progression Tree
    FProgressionTree SurvivalTree;
    SurvivalTree.TreeName = TEXT("Wilderness Survival");
    SurvivalTree.Description = TEXT("Master the harsh wilderness and survive against all odds");
    SurvivalTree.TreeType = EProgressionTreeType::Survival;
    
    FProgressionNode Forager;
    Forager.NodeID = TEXT("Forager");
    Forager.NodeName = TEXT("Expert Forager");
    Forager.Description = TEXT("Find 50% more resources when gathering");
    Forager.RequiredLevel = 3;
    Forager.SkillPointCost = 2;
    Forager.Prerequisites.Empty();
    Forager.bIsUnlocked = false;
    SurvivalTree.Nodes.Add(TEXT("Forager"), Forager);
    
    FProgressionNode IronStomach;
    IronStomach.NodeID = TEXT("IronStomach");
    IronStomach.NodeName = TEXT("Iron Stomach");
    IronStomach.Description = TEXT("Immune to food poisoning and hunger depletes 25% slower");
    IronStomach.RequiredLevel = 6;
    IronStomach.SkillPointCost = 3;
    IronStomach.Prerequisites.Add(TEXT("Forager"));
    IronStomach.bIsUnlocked = false;
    SurvivalTree.Nodes.Add(TEXT("IronStomach"), IronStomach);
    
    FProgressionNode WildernessExpert;
    WildernessExpert.NodeID = TEXT("WildernessExpert");
    WildernessExpert.NodeName = TEXT("Wilderness Expert");
    WildernessExpert.Description = TEXT("Move 25% faster in wilderness areas and take 50% less environmental damage");
    WildernessExpert.RequiredLevel = 10;
    WildernessExpert.SkillPointCost = 4;
    WildernessExpert.Prerequisites.Add(TEXT("IronStomach"));
    WildernessExpert.bIsUnlocked = false;
    SurvivalTree.Nodes.Add(TEXT("WildernessExpert"), WildernessExpert);
    
    ProgressionTrees.Add(TEXT("Survival"), SurvivalTree);
    
    // Social Progression Tree
    FProgressionTree SocialTree;
    SocialTree.TreeName = TEXT("Social Influence");
    SocialTree.Description = TEXT("Master the art of persuasion and social manipulation");
    SocialTree.TreeType = EProgressionTreeType::Social;
    
    FProgressionNode Charismatic;
    Charismatic.NodeID = TEXT("Charismatic");
    Charismatic.NodeName = TEXT("Natural Charisma");
    Charismatic.Description = TEXT("All NPCs start with +10 reputation towards you");
    Charismatic.RequiredLevel = 4;
    Charismatic.SkillPointCost = 3;
    Charismatic.Prerequisites.Empty();
    Charismatic.bIsUnlocked = false;
    SocialTree.Nodes.Add(TEXT("Charismatic"), Charismatic);
    
    FProgressionNode Diplomat;
    Diplomat.NodeID = TEXT("Diplomat");
    Diplomat.NodeName = TEXT("Master Diplomat");
    Diplomat.Description = TEXT("Faction reputation changes are 50% more effective");
    Diplomat.RequiredLevel = 7;
    Diplomat.SkillPointCost = 4;
    Diplomat.Prerequisites.Add(TEXT("Charismatic"));
    Diplomat.bIsUnlocked = false;
    SocialTree.Nodes.Add(TEXT("Diplomat"), Diplomat);
    
    FProgressionNode Manipulator;
    Manipulator.NodeID = TEXT("Manipulator");
    Manipulator.NodeName = TEXT("Master Manipulator");
    Manipulator.Description = TEXT("Can turn enemies into allies and cause faction conflicts");
    Manipulator.RequiredLevel = 11;
    Manipulator.SkillPointCost = 5;
    Manipulator.Prerequisites.Add(TEXT("Diplomat"));
    Manipulator.bIsUnlocked = false;
    SocialTree.Nodes.Add(TEXT("Manipulator"), Manipulator);
    
    ProgressionTrees.Add(TEXT("Social"), SocialTree);
    
    // Crafting Progression Tree
    FProgressionTree CraftingTree;
    CraftingTree.TreeName = TEXT("Master Craftsman");
    CraftingTree.Description = TEXT("Become a legendary artisan and creator");
    CraftingTree.TreeType = EProgressionTreeType::Crafting;
    
    FProgressionNode EfficientCrafter;
    EfficientCrafter.NodeID = TEXT("EfficientCrafter");
    EfficientCrafter.NodeName = TEXT("Efficient Crafting");
    EfficientCrafter.Description = TEXT("Crafting uses 25% fewer materials");
    EfficientCrafter.RequiredLevel = 3;
    EfficientCrafter.SkillPointCost = 2;
    EfficientCrafter.Prerequisites.Empty();
    EfficientCrafter.bIsUnlocked = false;
    CraftingTree.Nodes.Add(TEXT("EfficientCrafter"), EfficientCrafter);
    
    FProgressionNode QualityCrafter;
    QualityCrafter.NodeID = TEXT("QualityCrafter");
    QualityCrafter.NodeName = TEXT("Quality Crafting");
    QualityCrafter.Description = TEXT("25% chance to create higher quality items");
    QualityCrafter.RequiredLevel = 6;
    QualityCrafter.SkillPointCost = 3;
    QualityCrafter.Prerequisites.Add(TEXT("EfficientCrafter"));
    QualityCrafter.bIsUnlocked = false;
    CraftingTree.Nodes.Add(TEXT("QualityCrafter"), QualityCrafter);
    
    FProgressionNode LegendaryCrafter;
    LegendaryCrafter.NodeID = TEXT("LegendaryCrafter");
    LegendaryCrafter.NodeName = TEXT("Legendary Artisan");
    LegendaryCrafter.Description = TEXT("Can craft legendary items with unique properties");
    LegendaryCrafter.RequiredLevel = 15;
    LegendaryCrafter.SkillPointCost = 6;
    LegendaryCrafter.Prerequisites.Add(TEXT("QualityCrafter"));
    LegendaryCrafter.bIsUnlocked = false;
    CraftingTree.Nodes.Add(TEXT("LegendaryCrafter"), LegendaryCrafter);
    
    ProgressionTrees.Add(TEXT("Crafting"), CraftingTree);
    
    UE_LOG(LogTemp, Log, TEXT("Initialized %d progression trees"), ProgressionTrees.Num());
}

void UAdvancedPlayerProgressionSubsystem::InitializeAchievementSystem()
{
    // Combat Achievements
    FAchievement FirstBlood;
    FirstBlood.AchievementID = TEXT("FirstBlood");
    FirstBlood.Name = TEXT("First Blood");
    FirstBlood.Description = TEXT("Kill your first enemy");
    FirstBlood.Category = EAchievementCategory::Combat;
    FirstBlood.RequiredValue = 1;
    FirstBlood.CurrentProgress = 0;
    FirstBlood.bIsUnlocked = false;
    FirstBlood.ExperienceReward = 50.0f;
    FirstBlood.SkillPointReward = 1;
    Achievements.Add(TEXT("FirstBlood"), FirstBlood);
    
    FAchievement Slayer;
    Slayer.AchievementID = TEXT("Slayer");
    Slayer.Name = TEXT("Slayer");
    Slayer.Description = TEXT("Kill 100 enemies");
    Slayer.Category = EAchievementCategory::Combat;
    Slayer.RequiredValue = 100;
    Slayer.CurrentProgress = 0;
    Slayer.bIsUnlocked = false;
    Slayer.ExperienceReward = 500.0f;
    Slayer.SkillPointReward = 3;
    Achievements.Add(TEXT("Slayer"), Slayer);
    
    // Survival Achievements
    FAchievement Survivor;
    Survivor.AchievementID = TEXT("Survivor");
    Survivor.Name = TEXT("Survivor");
    Survivor.Description = TEXT("Survive for 7 consecutive days");
    Survivor.Category = EAchievementCategory::Survival;
    Survivor.RequiredValue = 7;
    Survivor.CurrentProgress = 0;
    Survivor.bIsUnlocked = false;
    Survivor.ExperienceReward = 200.0f;
    Survivor.SkillPointReward = 2;
    Achievements.Add(TEXT("Survivor"), Survivor);
    
    FAchievement MasterForager;
    MasterForager.AchievementID = TEXT("MasterForager");
    MasterForager.Name = TEXT("Master Forager");
    MasterForager.Description = TEXT("Gather 1000 resources");
    MasterForager.Category = EAchievementCategory::Survival;
    MasterForager.RequiredValue = 1000;
    MasterForager.CurrentProgress = 0;
    MasterForager.bIsUnlocked = false;
    MasterForager.ExperienceReward = 300.0f;
    MasterForager.SkillPointReward = 2;
    Achievements.Add(TEXT("MasterForager"), MasterForager);
    
    // Social Achievements
    FAchievement Diplomat;
    Diplomat.AchievementID = TEXT("DiplomatAchievement");
    Diplomat.Name = TEXT("Diplomat");
    Diplomat.Description = TEXT("Reach maximum reputation with any faction");
    Diplomat.Category = EAchievementCategory::Social;
    Diplomat.RequiredValue = 1;
    Diplomat.CurrentProgress = 0;
    Diplomat.bIsUnlocked = false;
    Diplomat.ExperienceReward = 400.0f;
    Diplomat.SkillPointReward = 3;
    Achievements.Add(TEXT("DiplomatAchievement"), Diplomat);
    
    FAchievement Peacemaker;
    Peacemaker.AchievementID = TEXT("Peacemaker");
    Peacemaker.Name = TEXT("Peacemaker");
    Peacemaker.Description = TEXT("Negotiate peace between warring factions");
    Peacemaker.Category = EAchievementCategory::Social;
    Peacemaker.RequiredValue = 1;
    Peacemaker.CurrentProgress = 0;
    Peacemaker.bIsUnlocked = false;
    Peacemaker.ExperienceReward = 600.0f;
    Peacemaker.SkillPointReward = 4;
    Achievements.Add(TEXT("Peacemaker"), Peacemaker);
    
    // Crafting Achievements
    FAchievement Artisan;
    Artisan.AchievementID = TEXT("Artisan");
    Artisan.Name = TEXT("Artisan");
    Artisan.Description = TEXT("Craft 100 items");
    Artisan.Category = EAchievementCategory::Crafting;
    Artisan.RequiredValue = 100;
    Artisan.CurrentProgress = 0;
    Artisan.bIsUnlocked = false;
    Artisan.ExperienceReward = 250.0f;
    Artisan.SkillPointReward = 2;
    Achievements.Add(TEXT("Artisan"), Artisan);
    
    FAchievement LegendarySmith;
    LegendarySmith.AchievementID = TEXT("LegendarySmith");
    LegendarySmith.Name = TEXT("Legendary Smith");
    LegendarySmith.Description = TEXT("Craft a legendary item");
    LegendarySmith.Category = EAchievementCategory::Crafting;
    LegendarySmith.RequiredValue = 1;
    LegendarySmith.CurrentProgress = 0;
    LegendarySmith.bIsUnlocked = false;
    LegendarySmith.ExperienceReward = 1000.0f;
    LegendarySmith.SkillPointReward = 5;
    Achievements.Add(TEXT("LegendarySmith"), LegendarySmith);
    
    // Exploration Achievements
    FAchievement Explorer;
    Explorer.AchievementID = TEXT("Explorer");
    Explorer.Name = TEXT("Explorer");
    Explorer.Description = TEXT("Discover 10 new locations");
    Explorer.Category = EAchievementCategory::Exploration;
    Explorer.RequiredValue = 10;
    Explorer.CurrentProgress = 0;
    Explorer.bIsUnlocked = false;
    Explorer.ExperienceReward = 300.0f;
    Explorer.SkillPointReward = 2;
    Achievements.Add(TEXT("Explorer"), Explorer);
    
    UE_LOG(LogTemp, Log, TEXT("Initialized %d achievements"), Achievements.Num());
}

void UAdvancedPlayerProgressionSubsystem::InitializeLegacySystem()
{
    // Legacy effects that persist across playthroughs or provide long-term benefits
    
    FLegacyEffect VeteranWarrior;
    VeteranWarrior.EffectID = TEXT("VeteranWarrior");
    VeteranWarrior.Name = TEXT("Veteran Warrior");
    VeteranWarrior.Description = TEXT("Your combat experience echoes through time");
    VeteranWarrior.EffectType = ELegacyEffectType::Combat;
    VeteranWarrior.RequiredAchievements.Add(TEXT("Slayer"));
    VeteranWarrior.bIsActive = false;
    VeteranWarrior.EffectMagnitude = 0.1f; // 10% combat bonus
    LegacyEffects.Add(TEXT("VeteranWarrior"), VeteranWarrior);
    
    FLegacyEffect WildernessLegend;
    WildernessLegend.EffectID = TEXT("WildernessLegend");
    WildernessLegend.Name = TEXT("Legend of the Wild");
    WildernessLegend.Description = TEXT("Your survival skills are legendary");
    WildernessLegend.EffectType = ELegacyEffectType::Survival;
    WildernessLegend.RequiredAchievements.Add(TEXT("Survivor"));
    WildernessLegend.RequiredAchievements.Add(TEXT("MasterForager"));
    WildernessLegend.bIsActive = false;
    WildernessLegend.EffectMagnitude = 0.15f; // 15% survival bonus
    LegacyEffects.Add(TEXT("WildernessLegend"), WildernessLegend);
    
    FLegacyEffect MasterDiplomat;
    MasterDiplomat.EffectID = TEXT("MasterDiplomat");
    MasterDiplomat.Name = TEXT("Master Diplomat");
    MasterDiplomat.Description = TEXT("Your diplomatic reputation precedes you");
    MasterDiplomat.EffectType = ELegacyEffectType::Social;
    MasterDiplomat.RequiredAchievements.Add(TEXT("DiplomatAchievement"));
    MasterDiplomat.RequiredAchievements.Add(TEXT("Peacemaker"));
    MasterDiplomat.bIsActive = false;
    MasterDiplomat.EffectMagnitude = 0.2f; // 20% reputation bonus
    LegacyEffects.Add(TEXT("MasterDiplomat"), MasterDiplomat);
    
    UE_LOG(LogTemp, Log, TEXT("Initialized %d legacy effects"), LegacyEffects.Num());
}

void UAdvancedPlayerProgressionSubsystem::UpdateProgressionTracking()
{
    // Track various player statistics for achievements and progression
    UpdateCombatStats();
    UpdateSurvivalStats();
    UpdateSocialStats();
    UpdateCraftingStats();
    UpdateExplorationStats();
}

void UAdvancedPlayerProgressionSubsystem::UpdateCombatStats()
{
    // This would integrate with combat system to track kills, damage dealt, etc.
    // For now, simulate some progress
    if (FMath::FRand() < 0.1f) // 10% chance per update
    {
        UpdateAchievementProgress(TEXT("FirstBlood"), 1);
        UpdateAchievementProgress(TEXT("Slayer"), 1);
        
        UE_LOG(LogTemp, Log, TEXT("Combat stats updated"));
    }
}

void UAdvancedPlayerProgressionSubsystem::UpdateSurvivalStats()
{
    // Track survival-related activities
    if (FMath::FRand() < 0.2f) // 20% chance per update
    {
        UpdateAchievementProgress(TEXT("MasterForager"), FMath::RandRange(1, 5));
        
        // Track consecutive survival days
        if (UGameInstance* GameInstance = GetGameInstance())
        {
            if (UWorld* World = GameInstance->GetWorld())
            {
                const float CurrentTime = World->GetTimeSeconds();
                const float DaysSurvived = CurrentTime / 86400.0f; // Assuming 1 day = 24 hours real time
                UpdateAchievementProgress(TEXT("Survivor"), static_cast<int32>(DaysSurvived));
            }
        }
        UE_LOG(LogTemp, Log, TEXT("Survival stats updated"));
    }
}

void UAdvancedPlayerProgressionSubsystem::UpdateSocialStats()
{
    // Track social interactions and reputation changes
    // This would integrate with faction system
    if (FMath::FRand() < 0.05f) // 5% chance per update
    {
        // Simulate reaching max reputation with a faction
        if (FMath::FRand() < 0.1f)
        {
            UpdateAchievementProgress(TEXT("DiplomatAchievement"), 1);
        }
        
        // Simulate peace negotiations
        if (FMath::FRand() < 0.05f)
        {
            UpdateAchievementProgress(TEXT("Peacemaker"), 1);
        }
        
        UE_LOG(LogTemp, Log, TEXT("Social stats updated"));
    }
}

void UAdvancedPlayerProgressionSubsystem::UpdateCraftingStats()
{
    // Track crafting activities
    if (FMath::FRand() < 0.15f) // 15% chance per update
    {
        UpdateAchievementProgress(TEXT("Artisan"), FMath::RandRange(1, 3));
        
        // Rare chance to craft legendary item
        if (FMath::FRand() < 0.01f)
        {
            UpdateAchievementProgress(TEXT("LegendarySmith"), 1);
        }
        
        UE_LOG(LogTemp, Log, TEXT("Crafting stats updated"));
    }
}

void UAdvancedPlayerProgressionSubsystem::UpdateExplorationStats()
{
    // Track exploration and discovery
    if (FMath::FRand() < 0.08f) // 8% chance per update
    {
        UpdateAchievementProgress(TEXT("Explorer"), 1);
        
        UE_LOG(LogTemp, Log, TEXT("Exploration stats updated"));
    }
}

void UAdvancedPlayerProgressionSubsystem::UpdateAchievementProgress(const FString& AchievementID, int32 Progress)
{
    if (FAchievement* Achievement = Achievements.Find(AchievementID))
    {
        if (!Achievement->bIsUnlocked)
        {
            Achievement->CurrentProgress += Progress;
            
            if (Achievement->CurrentProgress >= Achievement->RequiredValue)
            {
                UnlockAchievement(AchievementID);
            }
        }
    }
}

void UAdvancedPlayerProgressionSubsystem::UnlockAchievement(const FString& AchievementID)
{
    if (FAchievement* Achievement = Achievements.Find(AchievementID))
    {
        if (!Achievement->bIsUnlocked)
        {
            Achievement->bIsUnlocked = true;
            
            // Award rewards
            AwardExperience(Achievement->ExperienceReward);
            AvailableSkillPoints += Achievement->SkillPointReward;
            
            UE_LOG(LogTemp, Warning, TEXT("Achievement Unlocked: %s - Awarded %f XP and %d skill points"), 
                *Achievement->Name, Achievement->ExperienceReward, Achievement->SkillPointReward);
            
            OnAchievementUnlocked.Broadcast(*Achievement);
            
            // Check if this unlocks any legacy effects
            CheckLegacyEffectUnlocks();
        }
    }
}

void UAdvancedPlayerProgressionSubsystem::CheckAchievements()
{
    // This function is called periodically to check for achievement unlocks
    // Most achievement progress is updated in real-time, but some might need periodic checks
    
    for (auto& AchievementPair : Achievements)
    {
        FAchievement& Achievement = AchievementPair.Value;
        
        if (!Achievement.bIsUnlocked && Achievement.CurrentProgress >= Achievement.RequiredValue)
        {
            UnlockAchievement(Achievement.AchievementID);
        }
    }
}

void UAdvancedPlayerProgressionSubsystem::CheckLegacyEffectUnlocks()
{
    for (auto& LegacyPair : LegacyEffects)
    {
        FLegacyEffect& Legacy = LegacyPair.Value;
        
        if (!Legacy.bIsActive)
        {
            bool bCanUnlock = true;
            
            // Check if all required achievements are unlocked
            for (const FString& RequiredAchievement : Legacy.RequiredAchievements)
            {
                if (FAchievement* Achievement = Achievements.Find(RequiredAchievement))
                {
                    if (!Achievement->bIsUnlocked)
                    {
                        bCanUnlock = false;
                        break;
                    }
                }
                else
                {
                    bCanUnlock = false;
                    break;
                }
            }
            
            if (bCanUnlock)
            {
                Legacy.bIsActive = true;
                UE_LOG(LogTemp, Warning, TEXT("Legacy Effect Activated: %s"), *Legacy.Name);
                OnLegacyEffectActivated.Broadcast(Legacy);
            }
        }
    }
}

void UAdvancedPlayerProgressionSubsystem::UpdateLegacyEffects()
{
    // Apply active legacy effects
    for (const auto& LegacyPair : LegacyEffects)
    {
        const FLegacyEffect& Legacy = LegacyPair.Value;
        
        if (Legacy.bIsActive)
        {
            ApplyLegacyEffect(Legacy);
        }
    }
}

void UAdvancedPlayerProgressionSubsystem::ApplyLegacyEffect(const FLegacyEffect& LegacyEffect)
{
    // Apply the legacy effect based on its type
    switch (LegacyEffect.EffectType)
    {
    case ELegacyEffectType::Combat:
        // Apply combat bonuses (would integrate with combat system)
        UE_LOG(LogTemp, Log, TEXT("Applying combat legacy effect: %f bonus"), LegacyEffect.EffectMagnitude);
        break;
        
    case ELegacyEffectType::Survival:
        // Apply survival bonuses (would integrate with survival system)
        UE_LOG(LogTemp, Log, TEXT("Applying survival legacy effect: %f bonus"), LegacyEffect.EffectMagnitude);
        break;
        
    case ELegacyEffectType::Social:
        // Apply social bonuses (would integrate with faction system)
        UE_LOG(LogTemp, Log, TEXT("Applying social legacy effect: %f bonus"), LegacyEffect.EffectMagnitude);
        break;
        
    case ELegacyEffectType::Crafting:
        // Apply crafting bonuses (would integrate with crafting system)
        UE_LOG(LogTemp, Log, TEXT("Applying crafting legacy effect: %f bonus"), LegacyEffect.EffectMagnitude);
        break;
    }
}

void UAdvancedPlayerProgressionSubsystem::UpdateActiveEffects(float DeltaTime)
{
    // Update any time-based progression effects
    for (auto& EffectPair : ActiveProgressionEffects)
    {
        FActiveProgressionEffect& Effect = EffectPair.Value;
        
        if (Effect.Duration > 0.0f)
        {
            Effect.RemainingTime -= DeltaTime;
            
            if (Effect.RemainingTime <= 0.0f)
            {
                // Effect expired
                UE_LOG(LogTemp, Log, TEXT("Progression effect expired: %s"), *Effect.EffectName);
                ActiveProgressionEffects.Remove(EffectPair.Key);
            }
        }
    }
}

// Public Interface Functions
void UAdvancedPlayerProgressionSubsystem::AwardExperience(float Experience)
{
    TotalExperience += Experience;
    
    // Check for level up
    float RequiredXP = CalculateRequiredExperienceForLevel(PlayerLevel + 1);
    
    while (TotalExperience >= RequiredXP)
    {
        LevelUp();
        RequiredXP = CalculateRequiredExperienceForLevel(PlayerLevel + 1);
    }
    
    UE_LOG(LogTemp, Log, TEXT("Awarded %f experience. Total: %f"), Experience, TotalExperience);
    OnExperienceGained.Broadcast(Experience, TotalExperience);
}

void UAdvancedPlayerProgressionSubsystem::LevelUp()
{
    PlayerLevel++;
    AvailableAttributePoints += 2; // 2 attribute points per level
    AvailableSkillPoints += 1; // 1 skill point per level
    
    UE_LOG(LogTemp, Warning, TEXT("LEVEL UP! New level: %d"), PlayerLevel);
    OnPlayerLevelUp.Broadcast(PlayerLevel, AvailableAttributePoints, AvailableSkillPoints);
}

float UAdvancedPlayerProgressionSubsystem::CalculateRequiredExperienceForLevel(int32 Level)
{
    // Exponential XP curve: XP = 100 * (Level^1.5)
    return 100.0f * FMath::Pow(Level, 1.5f);
}

bool UAdvancedPlayerProgressionSubsystem::UnlockProgressionNode(const FString& TreeName, const FString& NodeID)
{
    if (FProgressionTree* Tree = ProgressionTrees.Find(TreeName))
    {
        if (FProgressionNode* Node = Tree->Nodes.Find(NodeID))
        {
            if (Node->bIsUnlocked)
            {
                UE_LOG(LogTemp, Warning, TEXT("Node %s is already unlocked"), *NodeID);
                return false;
            }
            
            // Check prerequisites
            for (const FString& PrereqID : Node->Prerequisites)
            {
                if (FProgressionNode* PrereqNode = Tree->Nodes.Find(PrereqID))
                {
                    if (!PrereqNode->bIsUnlocked)
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Prerequisite %s not unlocked for node %s"), *PrereqID, *NodeID);
                        return false;
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("Prerequisite %s not found for node %s"), *PrereqID, *NodeID);
                    return false;
                }
            }
            
            // Check level requirement
            if (PlayerLevel < Node->RequiredLevel)
            {
                UE_LOG(LogTemp, Warning, TEXT("Player level %d insufficient for node %s (requires %d)"), PlayerLevel, *NodeID, Node->RequiredLevel);
                return false;
            }
            
            // Check skill points
            if (AvailableSkillPoints < Node->SkillPointCost)
            {
                UE_LOG(LogTemp, Warning, TEXT("Insufficient skill points for node %s (have %d, need %d)"), *NodeID, AvailableSkillPoints, Node->SkillPointCost);
                return false;
            }
            
            // Unlock the node
            Node->bIsUnlocked = true;
            AvailableSkillPoints -= Node->SkillPointCost;
            
            UE_LOG(LogTemp, Warning, TEXT("Unlocked progression node: %s in tree %s"), *NodeID, *TreeName);
            OnProgressionNodeUnlocked.Broadcast(TreeName, NodeID);
            
            return true;
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Node %s not found in tree %s"), *NodeID, *TreeName);
            return false;
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Progression tree %s not found"), *TreeName);
        return false;
    }
}

TArray<FAchievement> UAdvancedPlayerProgressionSubsystem::GetUnlockedAchievements() const
{
    TArray<FAchievement> UnlockedAchievements;
    
    for (const auto& AchievementPair : Achievements)
    {
        if (AchievementPair.Value.bIsUnlocked)
        {
            UnlockedAchievements.Add(AchievementPair.Value);
        }
    }
    
    return UnlockedAchievements;
}

TArray<FLegacyEffect> UAdvancedPlayerProgressionSubsystem::GetActiveLegacyEffects() const
{
    TArray<FLegacyEffect> ActiveEffects;
    
    for (const auto& LegacyPair : LegacyEffects)
    {
        if (LegacyPair.Value.bIsActive)
        {
            ActiveEffects.Add(LegacyPair.Value);
        }
    }
    
    return ActiveEffects;
}

FProgressionTree UAdvancedPlayerProgressionSubsystem::GetProgressionTree(const FString& TreeName) const
{
    if (const FProgressionTree* Tree = ProgressionTrees.Find(TreeName))
    {
        return *Tree;
    }
    
    return FProgressionTree();
}