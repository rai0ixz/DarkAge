#include "Core/AchievementManager.h"
#include "UI/DAUIManager.h"
#include "Core/PlayerProfileManager.h"
#include "Core/PlayerSaveGame.h"
#include "Data/AchievementData.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

void UAchievementManager::UnlockAchievement(FName AchievementID)
{
    bool bAlreadyUnlocked = IsAchievementUnlocked(AchievementID);

    if (!bAlreadyUnlocked)
    {
        FPlayerAchievement* FoundAch = Achievements.FindByPredicate([&](const FPlayerAchievement& Ach) {
            return Ach.AchievementID == AchievementID;
        });

        if (FoundAch)
        {
            FoundAch->bUnlocked = true;
            FoundAch->UnlockTime = FDateTime::Now();
        }
        else
        {
            FPlayerAchievement NewAch;
            NewAch.AchievementID = AchievementID;
            NewAch.bUnlocked = true;
            NewAch.UnlockTime = FDateTime::Now();
            Achievements.Add(NewAch);
        }

        // Notify UI
        if (UWorld* World = GetWorld())
        {
            if (UDAUIManager* UIManager = World->GetGameInstance()->GetSubsystem<UDAUIManager>())
            {
                // Get achievement data for the broadcast
                FAchievementData AchievementData;
                // For now, create a basic achievement data structure
                // In a full implementation, you would load this from a data table
                AchievementData.AchievementID = AchievementID;
                AchievementData.DisplayName = FText::FromName(AchievementID);
                AchievementData.Description = FText::FromString(TEXT("Achievement unlocked!"));

                UIManager->OnAchievementUnlocked.Broadcast(AchievementID, AchievementData);
            }

            // Save Progress
            if (UPlayerProfileManager* ProfileManager = UGameplayStatics::GetGameInstance(World)->GetSubsystem<UPlayerProfileManager>())
            {
                // ProfileManager->SavePlayerProfile();
            }
        }
    }
}

bool UAchievementManager::IsAchievementUnlocked(FName AchievementID) const
{
    for (const FPlayerAchievement& Ach : Achievements)
    {
        if (Ach.AchievementID == AchievementID)
        {
            return Ach.bUnlocked;
        }
    }
    return false;
}

const TArray<FPlayerAchievement>& UAchievementManager::GetAllAchievements() const
{
    return Achievements;
}
