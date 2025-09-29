#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "PlayerLegacyData.generated.h"

USTRUCT(BlueprintType)
struct DARKAGE_API FWorldEventData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Legacy|World Event")
    FGuid EventID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Legacy|World Event")
    FString EventName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Legacy|World Event")
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Legacy|World Event")
    FString PlayerResponsible;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Legacy|World Event")
    int64 Timestamp;
    FORCEINLINE FWorldEventData()
        : EventID()
        , EventName(TEXT(""))
        , Description(TEXT(""))
        , PlayerResponsible(TEXT(""))
        , Timestamp(0)
    {
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FLegacyData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Legacy")
    int32 LegacyPoints;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Legacy")
    TArray<FName> Titles;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Legacy")
    TArray<FGuid> SignificantActions;
    FORCEINLINE FLegacyData()
        : LegacyPoints(0)
    {
    }
};

USTRUCT(BlueprintType)
struct DARKAGE_API FFamilyLineage
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Legacy|Family")
    FString FamilyName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Legacy|Family")
    TMap<FString, FString> Members; // MemberID -> Role
    FORCEINLINE FFamilyLineage()
        : FamilyName(TEXT(""))
    {
    }
};