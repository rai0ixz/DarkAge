// Copyright (c) 2025 RaioCore
// Base test utilities for DarkAge automated tests

#pragma once

#include "Misc/AutomationTest.h"
#include "GameFramework/Actor.h"

/**
 * Utility macros and functions for DarkAge testing
 */
namespace DATestUtils
{
    /**
     * Create a test actor with a specific component
     */
    template<typename ComponentType>
    ComponentType* CreateTestActorWithComponent()
    {
        AActor* TestActor = NewObject<AActor>();
        ComponentType* Component = NewObject<ComponentType>(TestActor);
        Component->RegisterComponent();
        return Component;
    }

    /**
     * Verify that a subsystem exists in the game instance
     */
    template<typename SubsystemType>
    bool VerifySubsystemExists(FAutomationTestBase* Test, const FString& SubsystemName)
    {
        if (!GWorld || !GWorld->GetGameInstance())
        {
            Test->AddError(FString::Printf(TEXT("No valid world or game instance for %s test"), *SubsystemName));
            return false;
        }

        SubsystemType* Subsystem = GWorld->GetGameInstance()->GetSubsystem<SubsystemType>();
        if (!Subsystem)
        {
            Test->AddError(FString::Printf(TEXT("%s subsystem not found"), *SubsystemName));
            return false;
        }

        Test->AddInfo(FString::Printf(TEXT("%s subsystem exists and is accessible"), *SubsystemName));
        return true;
    }
}

/**
 * Macro to create a simple subsystem existence test
 */
#define IMPLEMENT_SUBSYSTEM_TEST(TestName, SubsystemClass, SubsystemName) \
    IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestName, "DarkAge.Subsystems." SubsystemName, \
        EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter) \
    bool TestName::RunTest(const FString& Parameters) \
    { \
        return DATestUtils::VerifySubsystemExists<SubsystemClass>(this, SubsystemName); \
    }

/**
 * Macro to create a component creation test
 */
#define IMPLEMENT_COMPONENT_TEST(TestName, ComponentClass, ComponentName) \
    IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestName, "DarkAge.Components." ComponentName, \
        EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter) \
    bool TestName::RunTest(const FString& Parameters) \
    { \
        ComponentClass* Component = DATestUtils::CreateTestActorWithComponent<ComponentClass>(); \
        if (!Component) \
        { \
            AddError(TEXT("Failed to create " ComponentName " component")); \
            return false; \
        } \
        \
        AddInfo(TEXT(ComponentName " component created successfully")); \
        return true; \
    }