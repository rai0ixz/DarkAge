#pragma once

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

// If AutomationCommon.h is missing in your UE version, comment out or remove the include below.
// #include "AutomationCommon.h"

#if WITH_EDITOR
class FTemporalManagerTimeLoopTest : public FAutomationTestBase
{
public:
    FTemporalManagerTimeLoopTest(const FString& InName, bool bInComplexTask)
        : FAutomationTestBase(InName, bInComplexTask) {}

    virtual EAutomationTestFlags GetTestFlags() const override;
    virtual bool IsStressTest() const { return false; }
    virtual uint32 GetRequiredDeviceNum() const override { return 1; }
    virtual FString GetBeautifiedTestName() const override;
    virtual void GetTests(TArray<FString>& OutBeautifiedNames, TArray <FString>& OutTestCommands) const override;
    virtual bool RunTest(const FString& Parameters) override;
};
#endif // WITH_EDITOR