#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Core/WorldEcosystemSubsystem.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

#if WITH_EDITOR
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWorldEcosystemSubsystem_BasicTest, "DarkAge.WorldEcosystemSubsystem.Basic", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWorldEcosystemSubsystem_BasicTest::RunTest(const FString& Parameters)
{
    UWorld* World = GWorld;
    if (!World)
    {
        AddError(TEXT("No valid UWorld context for test."));
        return false;
    }
    UGameInstance* GameInstance = World->GetGameInstance();
    if (!GameInstance)
    {
        AddError(TEXT("No valid GameInstance for test."));
        return false;
    }
    UWorldEcosystemSubsystem* Ecosystem = GameInstance->GetSubsystem<UWorldEcosystemSubsystem>();
    if (!Ecosystem)
    {
        AddError(TEXT("Could not get UWorldEcosystemSubsystem."));
        return false;
    }
    // Register a region and check existence
    FString TestRegion = TEXT("TestRegion");
    Ecosystem->RegisterRegion(TestRegion, TEXT("Temperate"));
    TestTrue(TEXT("Region should be registered"), Ecosystem->HasRegion(TestRegion));
    // Check default season
    ESeasonType Season = Ecosystem->GetCurrentSeason(TestRegion);
    TestTrue(TEXT("Default season should be Spring"), Season == ESeasonType::Spring);
    // Add and check animal population
    Ecosystem->AddAnimalSpecies(TestRegion, TEXT("Wolf"), 10);
    int32 Pop = Ecosystem->GetAnimalPopulation(TestRegion, TEXT("Wolf"));
    TestTrue(TEXT("Wolf population should be 10"), Pop == 10);
    // Deplete and add resources
    Ecosystem->DepleteResource(TestRegion, EBasicResourceType::Water, 0.5f);
    float Water = Ecosystem->GetResourceAvailability(TestRegion, EBasicResourceType::Water);
    TestTrue(TEXT("Water should be less than max"), Water < 1.0f);
    Ecosystem->AddResource(TestRegion, EBasicResourceType::Water, 0.5f);
    Water = Ecosystem->GetResourceAvailability(TestRegion, EBasicResourceType::Water);
    TestTrue(TEXT("Water should be restored to max"), FMath::IsNearlyEqual(Water, 1.0f));
    // Unregister region
    Ecosystem->UnregisterRegion(TestRegion);
    TestFalse(TEXT("Region should be unregistered"), Ecosystem->HasRegion(TestRegion));
    return true;
}
#endif
