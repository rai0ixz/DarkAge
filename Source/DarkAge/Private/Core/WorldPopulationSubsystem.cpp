#include "Core/WorldPopulationSubsystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

UWorldPopulationSubsystem::UWorldPopulationSubsystem() {}

void UWorldPopulationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (!bPopulationEnabled)
    {
        return;
    }

    if (PopulationMode == EWorldPopulationMode::Manual)
    {
        ScanForManualWorldObjects();
    }
    else
    {
        SpawnProceduralWorldObjects();
    }
}

void UWorldPopulationSubsystem::Deinitialize()
{
    Super::Deinitialize();
    Towns.Empty();
    WaterSources.Empty();
    Housing.Empty();
    FoodSources.Empty();
    AllWorldObjects.Empty();
}

void UWorldPopulationSubsystem::SetPopulationMode(EWorldPopulationMode Mode)
{
    PopulationMode = Mode;
}

EWorldPopulationMode UWorldPopulationSubsystem::GetPopulationMode() const
{
    return PopulationMode;
}

void UWorldPopulationSubsystem::RegisterManualWorldObject(const FWorldObjectInfo& ObjectInfo)
{
    RegisterWorldObject(ObjectInfo);
}

void UWorldPopulationSubsystem::ProceduralGenerateWorldObjects()
{
    SpawnProceduralWorldObjects();
}

TArray<FWorldObjectInfo> UWorldPopulationSubsystem::GetWorldObjectsByType(const FString& Type) const
{
    if (Type == "Town") return Towns;
    if (Type == "Water") return WaterSources;
    if (Type == "Housing") return Housing;
    if (Type == "Food") return FoodSources;
    return {};
}

TArray<FWorldObjectInfo> UWorldPopulationSubsystem::GetAllWorldObjects() const
{
    return AllWorldObjects;
}

void UWorldPopulationSubsystem::ScanForManualWorldObjects()
{
    UWorld* World = GetWorld();
    if (!World) return;
    // Example: Find all actors tagged as Town, Water, Housing, Food
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithTag(World, FName("Town"), FoundActors);
    for (AActor* Actor : FoundActors)
    {
        FWorldObjectInfo Info;
        Info.Name = Actor->GetName();
        Info.Location = Actor->GetActorLocation();
        Info.ActorClass = Actor->GetClass();
        Info.ActorInstance = Actor;
        Towns.Add(Info);
        AllWorldObjects.Add(Info);
    }
    // Repeat for Water, Housing, Food
    FoundActors.Empty();
    UGameplayStatics::GetAllActorsWithTag(World, FName("Water"), FoundActors);
    for (AActor* Actor : FoundActors)
    {
        FWorldObjectInfo Info;
        Info.Name = Actor->GetName();
        Info.Location = Actor->GetActorLocation();
        Info.ActorClass = Actor->GetClass();
        Info.ActorInstance = Actor;
        WaterSources.Add(Info);
        AllWorldObjects.Add(Info);
    }
    FoundActors.Empty();
    UGameplayStatics::GetAllActorsWithTag(World, FName("Housing"), FoundActors);
    for (AActor* Actor : FoundActors)
    {
        FWorldObjectInfo Info;
        Info.Name = Actor->GetName();
        Info.Location = Actor->GetActorLocation();
        Info.ActorClass = Actor->GetClass();
        Info.ActorInstance = Actor;
        Housing.Add(Info);
        AllWorldObjects.Add(Info);
    }
    FoundActors.Empty();
    UGameplayStatics::GetAllActorsWithTag(World, FName("Food"), FoundActors);
    for (AActor* Actor : FoundActors)
    {
        FWorldObjectInfo Info;
        Info.Name = Actor->GetName();
        Info.Location = Actor->GetActorLocation();
        Info.ActorClass = Actor->GetClass();
        Info.ActorInstance = Actor;
        FoodSources.Add(Info);
        AllWorldObjects.Add(Info);
    }
}

void UWorldPopulationSubsystem::SpawnProceduralWorldObjects()
{
    UWorld* World = GetWorld();
    if (!World) return;
    // Example: Spawn towns, water, housing, food at random locations
    // This should be replaced with your own procedural logic
    for (int i = 0; i < 3; ++i)
    {
        FVector Location = FVector(FMath::RandRange(-5000, 5000), FMath::RandRange(-5000, 5000), 0);
        FWorldObjectInfo Info;
        Info.Name = FString::Printf(TEXT("Town_%d"), i);
        Info.Location = Location;
        // Info.ActorClass = ...; // Assign your town actor class
        // Info.ActorInstance = World->SpawnActor<AActor>(Info.ActorClass, Location, FRotator::ZeroRotator);
        Towns.Add(Info);
        AllWorldObjects.Add(Info);
    }
    // Repeat for Water, Housing, Food
}

void UWorldPopulationSubsystem::RegisterWorldObject(const FWorldObjectInfo& ObjectInfo)
{
    if (ObjectInfo.Name.Contains("Town")) Towns.Add(ObjectInfo);
    else if (ObjectInfo.Name.Contains("Water")) WaterSources.Add(ObjectInfo);
    else if (ObjectInfo.Name.Contains("Housing")) Housing.Add(ObjectInfo);
    else if (ObjectInfo.Name.Contains("Food")) FoodSources.Add(ObjectInfo);
    AllWorldObjects.Add(ObjectInfo);
}
