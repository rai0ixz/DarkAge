#if WITH_EDITOR
#include "TemporalManagerTest.h"
#include "Misc/AutomationTest.h"
#include "Engine/Engine.h"
#include "Core/TemporalManager.h"
#include "Core/TimeSystem.h"

EAutomationTestFlags FTemporalManagerTimeLoopTest::GetTestFlags() const
{
    return EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter;
}

FString FTemporalManagerTimeLoopTest::GetBeautifiedTestName() const
{
    return "DarkAge.Core.TemporalManager.TimeLoop";
}

void FTemporalManagerTimeLoopTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray <FString>& OutTestCommands) const
{
    OutBeautifiedNames.Add(GetBeautifiedTestName());
    OutTestCommands.Add(FString());
}

bool FTemporalManagerTimeLoopTest::RunTest(const FString& Parameters)
{
    // We need a world to get game instance and subsystems
    UWorld* World = GEngine->GetWorldContexts()[0].World();
    if (!TestNotNull("World should exist", World))
    {
        return false;
    }

    UGameInstance* GameInstance = World->GetGameInstance();
    if (!TestNotNull("GameInstance should exist", GameInstance))
    {
        return false;
    }

    // Get the subsystems
    UTemporalManager* TemporalManager = GameInstance->GetSubsystem<UTemporalManager>();
    if (!TestNotNull("TemporalManager should not be null", TemporalManager))
    {
        return false;
    }

    UTimeSystem* TimeSystem = GameInstance->GetSubsystem<UTimeSystem>();
    if (!TestNotNull("TimeSystem should not be null", TimeSystem))
    {
        return false;
    }

    // Define the time loop boundaries
    FGameDateTime StartTime(50, 1, 1, 8, 0);
    FGameDateTime EndTime(50, 1, 1, 8, 10);
    FGameDateTime TimeAfterLoop = FGameDateTime(50, 1, 1, 8, 11);

    // --- Test Case 1: Start and check active state ---
    TemporalManager->StartGlobalTimeLoop(StartTime, EndTime);
    TestTrue("Time loop should be active after starting", TemporalManager->IsTimeLoopActive());

    // --- Test Case 2: Trigger the loop reset ---
    // Set the time to be after the loop's end point
    TimeSystem->SetCurrentDateTime(TimeAfterLoop);

    // The HandleTimeUpdated should have fired and reset the time.
    TestTrue("Time should be reset to the start of the loop", TimeSystem->GetCurrentDateTime() == StartTime);

    // --- Test Case 3: Stop the loop ---
    TemporalManager->StopGlobalTimeLoop();
    TestFalse("Time loop should be inactive after stopping", TemporalManager->IsTimeLoopActive());

    // --- Test Case 4: Verify loop doesn't trigger when stopped ---
    // Set the time past the end point again
    TimeSystem->SetCurrentDateTime(TimeAfterLoop);

    // Time should NOT be reset this time
    TestTrue("Time should not reset when loop is inactive", TimeSystem->GetCurrentDateTime() == TimeAfterLoop);

    return true;
}

namespace
{
    FTemporalManagerTimeLoopTest FtmTimeLoopTest(TEXT("FTemporalManagerTimeLoopTest"), false);
}
#endif // WITH_EDITOR