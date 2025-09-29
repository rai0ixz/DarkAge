// Copyright (c) 2025 RaioCore
// Basic unit test for StatlineComponent (health, disease, hunger, thirst)
// Unreal Automation Test Example

#include "Misc/AutomationTest.h"
#include "Components/StatlineComponent.h"
#include "GameFramework/Actor.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FStatlineComponentBasicTest, "DarkAge.Statline.Basic", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FStatlineComponentBasicTest::RunTest(const FString& Parameters)
{
    // Create a dummy actor
    AActor* DummyActor = NewObject<AActor>();
    UStatlineComponent* Statline = NewObject<UStatlineComponent>(DummyActor);
    Statline->RegisterComponent();

    // Test initial health
    TestEqual(TEXT("Initial Health is MaxHealth"), Statline->GetCurrentStatValue(FName("Health")), Statline->GetMaxStatValue(FName("Health")));

    // Test health reduction
    Statline->UpdateStat(FName("Health"), -20.0f);
    TestEqual(TEXT("Health reduced by 20"), Statline->GetCurrentStatValue(FName("Health")), Statline->GetMaxStatValue(FName("Health")) - 20.0f);

    // Test status effect application
    // This will require a data table with a "TheShakes" status effect
    // For now, we will just check if the function can be called without crashing
    // FDataTableRowHandle TheShakesEffect;
    // TheShakesEffect.DataTable = ...;
    // TheShakesEffect.RowName = "TheShakes";
    // Statline->ApplyStatusEffect(TheShakesEffect, DummyActor);
    // TestTrue(TEXT("Has Status Effect: The Shakes"), Statline->HasStatusEffect("TheShakes"));

    // Test hunger and thirst
    float HungerBefore = Statline->GetCurrentStatValue(FName("Hunger"));
    Statline->UpdateStat(FName("Hunger"), -10.0f);
    TestEqual(TEXT("Hunger reduced by 10"), Statline->GetCurrentStatValue(FName("Hunger")), HungerBefore - 10.0f);

    float ThirstBefore = Statline->GetCurrentStatValue(FName("Thirst"));
    Statline->UpdateStat(FName("Thirst"), -5.0f);
    TestEqual(TEXT("Thirst reduced by 5"), Statline->GetCurrentStatValue(FName("Thirst")), ThirstBefore - 5.0f);

    return true;
}
