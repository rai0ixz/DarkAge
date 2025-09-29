#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CultureSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FCultureTrait
{
    GENERATED_BODY();
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TraitName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Value;
    friend FArchive& operator<<(FArchive& Ar, FCultureTrait& Trait)
    {
        Ar << Trait.TraitName;
        Ar << Trait.Value;
        return Ar;
    }
};

USTRUCT(BlueprintType)
struct FCultureProfile
{
    GENERATED_BODY();
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FCultureTrait> Traits;

    friend FArchive& operator<<(FArchive& Ar, FCultureProfile& Profile)
    {
        Ar << Profile.Traits;
        return Ar;
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCultureEvolved, FName, FactionID, const FCultureProfile&, NewProfile);

UCLASS()
class DARKAGE_API UCultureSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "Culture")
    void EvolveCultures();
    UFUNCTION(BlueprintCallable, Category = "Culture")
    void MergeTraits(FName FactionA, FName FactionB);
    UFUNCTION(BlueprintCallable, Category = "Culture")
    FCultureProfile GetCultureProfile(FName FactionID) const;

    UPROPERTY(BlueprintAssignable, Category = "Culture")
    FOnCultureEvolved OnCultureEvolved;
protected:
    UPROPERTY()
    TMap<FName, FCultureProfile> FactionCultures;

private:
    void RegisterDebugCommands();
    void SaveCultures() const;
    void LoadCultures();
};
