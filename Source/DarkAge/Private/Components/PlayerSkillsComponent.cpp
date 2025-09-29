// PlayerSkillsComponent.cpp

#include "Components/PlayerSkillsComponent.h"
#include "Data/PlayerSkillData.h" // Ensure this is included
#include "Engine/DataTable.h"
#include "Logging/LogMacros.h"

UPlayerSkillsComponent::UPlayerSkillsComponent()
{
    PrimaryComponentTick.bCanEverTick = false; // No tick needed by default
    LoadedSkillDefinitionsDataTable = nullptr;
}

void UPlayerSkillsComponent::BeginPlay()
{
    Super::BeginPlay();

    // Load Skill Definitions Data Table
    if (SkillDefinitionsDataTablePtr.IsNull())
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerSkillsComponent: SkillDefinitionsDataTablePtr is not set. Skill names and descriptions might be unavailable."));
    }
    else
    {
        LoadedSkillDefinitionsDataTable = SkillDefinitionsDataTablePtr.LoadSynchronous();
        if (!LoadedSkillDefinitionsDataTable)
        {
            UE_LOG(LogTemp, Error, TEXT("PlayerSkillsComponent: Failed to load SkillDefinitionsDataTable from path: %s"), *SkillDefinitionsDataTablePtr.ToSoftObjectPath().ToString());
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("PlayerSkillsComponent: SkillDefinitionsDataTable '%s' loaded successfully."), *LoadedSkillDefinitionsDataTable->GetName());
            InitializeAllDefinedSkills(); // Initialize all skills defined in the table
        }
    }

    // Load Perk Definitions Data Table
    if (PerkDefinitionsDataTablePtr.IsNull())
    {
        UE_LOG(LogTemp, Warning, TEXT("PlayerSkillsComponent: PerkDefinitionsDataTablePtr is not set. Perk effects will not be applied."));
    }
    else
    {
        LoadedPerkDefinitionsDataTable = PerkDefinitionsDataTablePtr.LoadSynchronous();
        if (!LoadedPerkDefinitionsDataTable)
        {
            UE_LOG(LogTemp, Error, TEXT("PlayerSkillsComponent: Failed to load PerkDefinitionsDataTable from path: %s"), *PerkDefinitionsDataTablePtr.ToSoftObjectPath().ToString());
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("PlayerSkillsComponent: PerkDefinitionsDataTable '%s' loaded successfully."), *LoadedPerkDefinitionsDataTable->GetName());
        }
    }
}

bool UPlayerSkillsComponent::GetSkillDefinition(FName SkillID, FSkillDefinition& OutSkillDef) const
{
    if (!LoadedSkillDefinitionsDataTable)
    {
        // UE_LOG(LogTemp, Warning, TEXT("GetSkillDefinition: LoadedSkillDefinitionsDataTable is NULL for SkillID: %s"), *SkillID.ToString());
        return false;
    }
    FString ContextString(TEXT("Getting Skill Definition"));
    FSkillDefinition* Row = LoadedSkillDefinitionsDataTable->FindRow<FSkillDefinition>(SkillID, ContextString);
    if (Row)
    {
        OutSkillDef = *Row;
        return true;
    }
    // UE_LOG(LogTemp, Warning, TEXT("GetSkillDefinition: SkillID '%s' not found in SkillDefinitionsDataTable."), *SkillID.ToString());
    return false;
}

float UPlayerSkillsComponent::CalculateXPForNextLevel(FName SkillID, int32 CurrentSkillLevel) const
{
    FSkillDefinition SkillDef;
    if (GetSkillDefinition(SkillID, SkillDef))
    {
        if (CurrentSkillLevel < 1) CurrentSkillLevel = 1; // Should not happen if initialized
        if (CurrentSkillLevel >= SkillDef.MaxLevel)
        {
            return 0; // Max level reached, no more XP needed
        }
        // Example XP curve: BaseXPForLevel2 * (XPIncreaseFactorPerLevel ^ (CurrentLevel - 1))
        // Level 1 -> 2 needs BaseXPForLevel2
        // Level 2 -> 3 needs BaseXPForLevel2 * XPIncreaseFactorPerLevel
        // Level L -> L+1 needs BaseXPForLevel2 * (XPIncreaseFactorPerLevel ^ (L - 1))
        return SkillDef.BaseXPForLevel2 * FMath::Pow(SkillDef.XPIncreaseFactorPerLevel, static_cast<float>(CurrentSkillLevel - 1));
    }
    return 100.0f; // Default if no definition found
}

void UPlayerSkillsComponent::InitializeSkill(FName SkillID, bool bForceReinitialize)
{
    if (SkillID == NAME_None) return;

    if (!PlayerSkills.Contains(SkillID) || bForceReinitialize)
    {
        FSkillDefinition SkillDef;
        float InitialXPToNext = 100.0f; // Default
        if (GetSkillDefinition(SkillID, SkillDef))
        {
            InitialXPToNext = CalculateXPForNextLevel(SkillID, 1);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("InitializeSkill: No definition found for SkillID '%s'. Initializing with default XP curve."), *SkillID.ToString());
        }

        FPlayerSkillInstanceData NewSkillData(SkillID, 1, 0.0f, InitialXPToNext);
        PlayerSkills.Add(SkillID, NewSkillData);
        UE_LOG(LogTemp, Log, TEXT("PlayerSkillsComponent: Initialized skill '%s' at Level 1, XP 0/%f."), *SkillID.ToString(), InitialXPToNext);
    }
}

void UPlayerSkillsComponent::InitializeAllDefinedSkills()
{
    if (!LoadedSkillDefinitionsDataTable) return;

    TArray<FName> RowNames = LoadedSkillDefinitionsDataTable->GetRowNames();
    for (FName RowName : RowNames)
    {
        InitializeSkill(RowName);
    }
}


int32 UPlayerSkillsComponent::GetSkillLevel(FName SkillID) const
{
    const FPlayerSkillInstanceData* SkillData = PlayerSkills.Find(SkillID);
    if (SkillData)
    {
        return SkillData->CurrentLevel;
    }
    // UE_LOG(LogTemp, Warning, TEXT("GetSkillLevel: SkillID '%s' not found in PlayerSkills."), *SkillID.ToString());
    return 0; // Or handle as error/default
}

float UPlayerSkillsComponent::GetSkillXP(FName SkillID) const
{
    const FPlayerSkillInstanceData* SkillData = PlayerSkills.Find(SkillID);
    if (SkillData)
    {
        return SkillData->CurrentXP;
    }
    return 0.0f;
}

float UPlayerSkillsComponent::GetXPToNextLevel(FName SkillID) const
{
    const FPlayerSkillInstanceData* SkillData = PlayerSkills.Find(SkillID);
    if (SkillData)
    {
        return SkillData->XPToNextLevel;
    }
    return 0.0f; // Or a very high number if skill not found
}

bool UPlayerSkillsComponent::GetSkillProgress(FName SkillID, float& OutCurrentXP, float& OutXPForNext, int32& OutCurrentLevel) const
{
    const FPlayerSkillInstanceData* SkillData = PlayerSkills.Find(SkillID);
    if (SkillData)
    {
        OutCurrentXP = SkillData->CurrentXP;
        OutXPForNext = SkillData->XPToNextLevel;
        OutCurrentLevel = SkillData->CurrentLevel;
        return true;
    }
    return false;
}

int32 UPlayerSkillsComponent::GetCharacterLevel() const
{
   if (PlayerSkills.Num() == 0)
   {
       return 1;
   }

   int32 TotalLevel = 0;
   for (const auto& SkillPair : PlayerSkills)
   {
       TotalLevel += SkillPair.Value.CurrentLevel;
   }

   return FMath::Max(1, TotalLevel / PlayerSkills.Num());
}

void UPlayerSkillsComponent::AddXP(FName SkillID, float XPAmount)
{
    if (XPAmount <= 0) return;

    FPlayerSkillInstanceData* SkillData = PlayerSkills.Find(SkillID);
    if (!SkillData)
    {
        UE_LOG(LogTemp, Warning, TEXT("AddXP: SkillID '%s' not found. Initializing it first."), *SkillID.ToString());
        InitializeSkill(SkillID); // Attempt to initialize if not found
        SkillData = PlayerSkills.Find(SkillID);
        if (!SkillData)
        {
            UE_LOG(LogTemp, Error, TEXT("AddXP: Failed to initialize SkillID '%s' after not being found."), *SkillID.ToString());
            return;
        }
    }

    FSkillDefinition SkillDef;
    bool bHasDef = GetSkillDefinition(SkillID, SkillDef);
    int32 MaxLevel = bHasDef ? SkillDef.MaxLevel : 100; // Default max level if no def

    if (SkillData->CurrentLevel >= MaxLevel && SkillData->XPToNextLevel == 0.f) // Max level reached
    {
        UE_LOG(LogTemp, Log, TEXT("AddXP: Skill '%s' is already at max level %d."), *SkillID.ToString(), MaxLevel);
        return;
    }

    SkillData->CurrentXP += XPAmount;
    UE_LOG(LogTemp, Log, TEXT("AddXP: Added %f XP to skill '%s'. New CurrentXP: %f / %f"), XPAmount, *SkillID.ToString(), SkillData->CurrentXP, SkillData->XPToNextLevel);
    OnSkillXPAdded_Event.Broadcast(SkillID, XPAmount, SkillData->CurrentXP);

    bool bLeveledUp = false;
    while (SkillData->CurrentLevel < MaxLevel && SkillData->CurrentXP >= SkillData->XPToNextLevel)
    {
        int32 OldLevel = SkillData->CurrentLevel;
        SkillData->CurrentXP -= SkillData->XPToNextLevel; // Subtract XP needed for this level
        SkillData->CurrentLevel++;
        bLeveledUp = true;

        if (SkillData->CurrentLevel >= MaxLevel)
        {
            SkillData->XPToNextLevel = 0; // Mark as maxed
            SkillData->CurrentXP = 0; // Optional: reset XP at max level or let it overflow
            UE_LOG(LogTemp, Log, TEXT("AddXP: Skill '%s' reached MAX LEVEL %d!"), *SkillID.ToString(), SkillData->CurrentLevel);
        }
        else
        {
            SkillData->XPToNextLevel = CalculateXPForNextLevel(SkillID, SkillData->CurrentLevel);
        }

        OnSkillLeveledUp_Event.Broadcast(SkillID, SkillData->CurrentLevel, OldLevel);
        UE_LOG(LogTemp, Log, TEXT("AddXP: Skill '%s' LEVELED UP to %d! XP: %f/%f"), *SkillID.ToString(), SkillData->CurrentLevel, SkillData->CurrentXP, SkillData->XPToNextLevel);
    }
}

// --- MODIFICATION START: Perk Management Function Implementations ---
bool UPlayerSkillsComponent::UnlockPerk(FName SkillID, FName PerkID)
{
    FPlayerSkillInstanceData* SkillData = PlayerSkills.Find(SkillID);
    if (!SkillData)
    {
        UE_LOG(LogTemp, Warning, TEXT("UnlockPerk: Cannot unlock perk '%s'. Skill '%s' not found."), *PerkID.ToString(), *SkillID.ToString());
        return false;
    }

    if (SkillData->UnlockedPerks.Contains(PerkID))
    {
        UE_LOG(LogTemp, Log, TEXT("UnlockPerk: Perk '%s' is already unlocked for skill '%s'."), *PerkID.ToString(), *SkillID.ToString());
        return false; // Already unlocked
    }

    FSkillDefinition SkillDef;
    if (!GetSkillDefinition(SkillID, SkillDef))
    {
        UE_LOG(LogTemp, Warning, TEXT("UnlockPerk: Could not find skill definition for skill '%s'."), *SkillID.ToString());
        return false;
    }

    FPerkDefinition* PerkDefRow = LoadedPerkDefinitionsDataTable->FindRow<FPerkDefinition>(PerkID, "");
    if (!PerkDefRow)
    {
        UE_LOG(LogTemp, Warning, TEXT("UnlockPerk: Could not find perk definition for perk '%s'."), *PerkID.ToString());
        return false;
    }

    FPerkDefinition PerkDef = *PerkDefRow;

    if (SkillData->CurrentLevel < PerkDef.RequiredSkillLevel)
    {
        UE_LOG(LogTemp, Warning, TEXT("UnlockPerk: Skill level %d is too low to unlock perk '%s' (requires %d)."), SkillData->CurrentLevel, *PerkID.ToString(), PerkDef.RequiredSkillLevel);
        return false;
    }

    for (const FName& RequiredPerk : PerkDef.RequiredPerkIDs)
    {
        if (!SkillData->UnlockedPerks.Contains(RequiredPerk))
        {
            UE_LOG(LogTemp, Warning, TEXT("UnlockPerk: Missing required perk '%s' to unlock '%s'."), *RequiredPerk.ToString(), *PerkID.ToString());
            return false;
        }
    }

    SkillData->UnlockedPerks.Add(PerkID);
    UE_LOG(LogTemp, Log, TEXT("UnlockPerk: Perk '%s' successfully unlocked for skill '%s'."), *PerkID.ToString(), *SkillID.ToString());
    OnPerkUnlocked_Event.Broadcast(SkillID, PerkID);
    return true;
}

bool UPlayerSkillsComponent::HasPerk(FName SkillID, FName PerkID) const
{
    const FPlayerSkillInstanceData* SkillData = PlayerSkills.Find(SkillID);
    if (SkillData)
    {
        return SkillData->UnlockedPerks.Contains(PerkID);
    }
    return false;
}
// --- MODIFICATION END ---

FText UPlayerSkillsComponent::GetSkillDisplayName(FName SkillID) const
{
    FSkillDefinition SkillDef;
    if (GetSkillDefinition(SkillID, SkillDef))
    {
        return SkillDef.DisplayName;
    }
    return FText::FromName(SkillID); // Fallback to SkillID
}

FText UPlayerSkillsComponent::GetSkillDescription(FName SkillID) const
{
    FSkillDefinition SkillDef;
    if (GetSkillDefinition(SkillID, SkillDef))
    {
        return SkillDef.Description;
    }
    return FText::GetEmpty();
}

bool UPlayerSkillsComponent::GetPlayerSkillInstanceData(FName SkillID, FPlayerSkillInstanceData& OutSkillData) const
{
    const FPlayerSkillInstanceData* SkillData = PlayerSkills.Find(SkillID);
    if (SkillData)
    {
        OutSkillData = *SkillData;
        return true;
    }
    return false;
}

const TMap<FName, FPlayerSkillInstanceData>& UPlayerSkillsComponent::GetAllSkillsData() const
{
    return PlayerSkills;
}

void UPlayerSkillsComponent::SetPlayerSkills(const TMap<FName, FPlayerSkillInstanceData>& InPlayerSkills)
{
    PlayerSkills = InPlayerSkills;
    for (const auto& Elem : PlayerSkills)
    {
        OnSkillLeveledUp_Event.Broadcast(Elem.Key, Elem.Value.CurrentLevel, Elem.Value.CurrentLevel);
    }
}

void UPlayerSkillsComponent::PrintAllSkills()
{
    UE_LOG(LogTemp, Log, TEXT("=== Player Skills ==="));
    
    if (PlayerSkills.Num() == 0)
    {
        UE_LOG(LogTemp, Log, TEXT("No skills available"));
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("Character Level: %d"), GetCharacterLevel());
        UE_LOG(LogTemp, Log, TEXT("Total Skills: %d"), PlayerSkills.Num());
        
        for (const auto& SkillPair : PlayerSkills)
        {
            const FName& SkillID = SkillPair.Key;
            const FPlayerSkillInstanceData& SkillData = SkillPair.Value;
            
            FText DisplayName = GetSkillDisplayName(SkillID);
            FString SkillName = DisplayName.IsEmpty() ? SkillID.ToString() : DisplayName.ToString();
            
            if (SkillData.XPToNextLevel > 0)
            {
                UE_LOG(LogTemp, Log, TEXT("%s: Level %d (%.1f/%.1f XP)"),
                    *SkillName,
                    SkillData.CurrentLevel,
                    SkillData.CurrentXP,
                    SkillData.XPToNextLevel);
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("%s: Level %d (MAX LEVEL)"),
                    *SkillName,
                    SkillData.CurrentLevel);
            }
            
            if (SkillData.UnlockedPerks.Num() > 0)
            {
                UE_LOG(LogTemp, Log, TEXT("  Perks: %d unlocked"), SkillData.UnlockedPerks.Num());
                for (const FName& PerkID : SkillData.UnlockedPerks)
                {
                    UE_LOG(LogTemp, Log, TEXT("    - %s"), *PerkID.ToString());
                }
            }
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("===================="));
}