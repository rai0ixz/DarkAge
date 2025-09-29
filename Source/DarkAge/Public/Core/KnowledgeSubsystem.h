#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KnowledgeSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FPrinciple
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> Inputs;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName Process;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName Result;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString DiscoveredBy;
};

// Delegate for new principle discovery
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPrincipleDiscovered, const FPrinciple&, Principle);

UCLASS()
class DARKAGE_API UKnowledgeSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "Knowledge")
    void RecordExperiment(const TArray<FName>& Inputs, FName Process, FName Result, const FString& PlayerID);

    UFUNCTION(BlueprintCallable, Category = "Knowledge")
    bool HasDiscoveredPrinciple(const TArray<FName>& Inputs, FName Process) const;

    // Blueprint-assignable event for new discoveries
    UPROPERTY(BlueprintAssignable, Category = "Knowledge")
    FOnPrincipleDiscovered OnPrincipleDiscovered;

protected:
    UPROPERTY()
    TArray<FPrinciple> DiscoveredPrinciples;
};
