#include "Core/CultureSubsystem.h"
#include "Core/GameDebugManagerSubsystem.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/ArchiveSaveCompressedProxy.h"
#include "Serialization/ArchiveLoadCompressedProxy.h"
#include "Kismet/GameplayStatics.h"

void UCultureSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    const FString ResetFlagPath = FPaths::ProjectSavedDir() + TEXT("ResetCultures.flag");
    if (IFileManager::Get().FileExists(*ResetFlagPath))
    {
        UE_LOG(LogTemp, Warning, TEXT("ResetCultures.flag found. Deleting Cultures.sav and re-initializing."));
        IFileManager::Get().Delete(*(FPaths::ProjectSavedDir() + TEXT("Cultures.sav")));
        IFileManager::Get().Delete(*ResetFlagPath);
    }

    LoadCultures();
}

void UCultureSubsystem::Deinitialize()
{
    SaveCultures();
    Super::Deinitialize();
}

void UCultureSubsystem::EvolveCultures()
{
    for (auto& Elem : FactionCultures)
    {
        FCultureProfile& Profile = Elem.Value;
        bool bCultureChanged = false;

        // Chance to gain a new trait
        if (FMath::FRand() < 0.1f)
        {
            FCultureTrait NewTrait;
            NewTrait.TraitName = FName(*FString::Printf(TEXT("Trait_%d"), FMath::RandRange(100, 999)));
            NewTrait.Value = FString::Printf(TEXT("Value_%d"), FMath::RandRange(0, 10));
            Profile.Traits.Add(NewTrait);
            bCultureChanged = true;
        }

        // Chance to lose a trait
        if (Profile.Traits.Num() > 1 && FMath::FRand() < 0.05f)
        {
            Profile.Traits.RemoveAt(FMath::RandRange(0, Profile.Traits.Num() - 1));
            bCultureChanged = true;
        }

        // Chance to modify a trait
        if (Profile.Traits.Num() > 0 && FMath::FRand() < 0.2f)
        {
            int32 TraitIndex = FMath::RandRange(0, Profile.Traits.Num() - 1);
            Profile.Traits[TraitIndex].Value = FString::Printf(TEXT("Value_%d"), FMath::RandRange(0, 10));
            bCultureChanged = true;
        }

        if (bCultureChanged)
        {
            OnCultureEvolved.Broadcast(Elem.Key, Profile);
        }
    }
}

void UCultureSubsystem::MergeTraits(FName FactionA, FName FactionB)
{
    if (FactionCultures.Contains(FactionA) && FactionCultures.Contains(FactionB))
    {
        FCultureProfile& ProfileA = FactionCultures[FactionA];
        const FCultureProfile& ProfileB = FactionCultures[FactionB];

        for (const FCultureTrait& TraitB : ProfileB.Traits)
        {
            bool bTraitExists = false;
            for (FCultureTrait& TraitA : ProfileA.Traits)
            {
                if (TraitA.TraitName == TraitB.TraitName)
                {
                    // Trait conflict: average the values
                    float ValueA = FCString::Atof(*TraitA.Value.RightChop(6));
                    float ValueB = FCString::Atof(*TraitB.Value.RightChop(6));
                    TraitA.Value = FString::Printf(TEXT("Value_%.2f"), (ValueA + ValueB) / 2.0f);
                    bTraitExists = true;
                    break;
                }
            }

            if (!bTraitExists)
            {
                ProfileA.Traits.Add(TraitB);
            }
        }
        OnCultureEvolved.Broadcast(FactionA, ProfileA);
    }
}

FCultureProfile UCultureSubsystem::GetCultureProfile(FName FactionID) const
{
    if (const FCultureProfile* Profile = FactionCultures.Find(FactionID))
    {
        return *Profile;
    }
    return FCultureProfile();
}


void UCultureSubsystem::SaveCultures() const
{
    TArray<uint8> SaveData;
    FMemoryWriter MemoryWriter(SaveData, true);
    
    int32 CultureCount = FactionCultures.Num();
    MemoryWriter << CultureCount;

    for (const auto& CulturePair : FactionCultures)
    {
        FName FactionID = CulturePair.Key;
        FCultureProfile CultureProfile = CulturePair.Value;
        MemoryWriter << FactionID;
        MemoryWriter << CultureProfile;
    }

    FString SavePath = FPaths::ProjectSavedDir() + TEXT("Cultures.sav");
    FFileHelper::SaveArrayToFile(SaveData, *SavePath);
}

void UCultureSubsystem::LoadCultures()
{
    TArray<uint8> LoadData;
    FString SavePath = FPaths::ProjectSavedDir() + TEXT("Cultures.sav");
    if (FFileHelper::LoadFileToArray(LoadData, *SavePath))
    {
        FMemoryReader MemoryReader(LoadData, true);
        
        int32 CultureCount;
        MemoryReader << CultureCount;

        for (int32 i = 0; i < CultureCount; ++i)
        {
            FName FactionID;
            FCultureProfile CultureProfile;
            MemoryReader << FactionID;
            MemoryReader << CultureProfile;
            FactionCultures.Add(FactionID, CultureProfile);
        }
    }
}
