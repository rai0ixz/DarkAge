using UnrealBuildTool;

public class DarkAge : ModuleRules
{
    public DarkAge(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "GameplayTags",
            "UMG",
            "Slate",
            "SlateCore",
            "AIModule",
            "NavigationSystem",
            "GameplayTasks",
            "Niagara",
            "PhysicsCore",
            "Chaos",
            "ChaosSolverEngine",
            "GeometryCollectionEngine",
            "FieldSystemEngine",
            "CableComponent",
            "ProceduralMeshComponent",
            "Landscape",
            "Foliage",
            "MediaAssets",
            "Json",
            "JsonUtilities",
            "HTTP",
            "WebSockets",
            "OnlineSubsystem",
            "OnlineSubsystemUtils",
            "Steamworks",
            "VoiceChat",
            "AudioMixer",
            "Synthesis",
            "AudioCapture",
            "SoundFieldRendering",
            "AudioExtensions",
            "RenderCore",
            "RHI",
            "ApplicationCore"
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Slate",
            "SlateCore",
            "UMG",
            "AIModule",
            "NavigationSystem",
            "GameplayTasks",
            "Niagara",
            "PhysicsCore",
            "Chaos",
            "ChaosSolverEngine",
            "GeometryCollectionEngine",
            "FieldSystemEngine",
            "CableComponent",
            "ProceduralMeshComponent",
            "Landscape",
            "Foliage",
            "MediaAssets",
            "Json",
            "JsonUtilities",
            "HTTP",
            "WebSockets",
            "OnlineSubsystem",
            "OnlineSubsystemUtils",
            "Steamworks",
            "VoiceChat",
            "AudioMixer",
            "Synthesis",
            "AudioCapture",
            "SoundFieldRendering",
            "AudioExtensions",
            "RenderCore", 
            "RHI",
            "ApplicationCore",
            "DeveloperSettings"
        });

        // Enable exceptions for better error handling
        bEnableExceptions = true;

        // Enable RTTI for dynamic casting
        bUseRTTI = true;

        // Enable unity builds for faster compilation
        bUseUnity = true;

        // Enable precompiled headers
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Enable IWYU (Include What You Use) for cleaner includes
        IWYUSupport = IWYUSupport.Full;

        // Define preprocessor macros
        PublicDefinitions.Add("DARK_AGE_VERSION=1");
        PublicDefinitions.Add("WITH_ADVANCED_AI=1");
        PublicDefinitions.Add("WITH_PROCEDURAL_GENERATION=1");
        PublicDefinitions.Add("WITH_MAGIC_SYSTEM=1");
        PublicDefinitions.Add("WITH_ADVANCED_ECONOMICS=1");
        PublicDefinitions.Add("WITH_MULTIPLAYER=1");
        PublicDefinitions.Add("WITH_PERFORMANCE_OPTIMIZATIONS=1");

        // Platform-specific settings
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            // Windows-specific optimizations
            PublicDefinitions.Add("PLATFORM_WINDOWS=1");
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            // Linux-specific settings
            PublicDefinitions.Add("PLATFORM_LINUX=1");
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            // Mac-specific settings
            PublicDefinitions.Add("PLATFORM_MAC=1");
        }

        // Configuration-specific settings
        if (Target.Configuration == UnrealTargetConfiguration.Development)
        {
            // Development build optimizations
            PublicDefinitions.Add("DARK_AGE_DEBUG=1");
        }
        else if (Target.Configuration == UnrealTargetConfiguration.Shipping)
        {
            // Shipping build optimizations
            PublicDefinitions.Add("DARK_AGE_SHIPPING=1");
        }

        // Plugin dependencies
        DynamicallyLoadedModuleNames.AddRange(new string[]
        {
            // Add any runtime-loaded plugins here
        });

        // Private includes
        PrivateIncludePaths.AddRange(new string[]
        {
            "DarkAge/Private",
            "DarkAge/Private/Core",
            "DarkAge/Private/Components",
            "DarkAge/Private/Characters",
            "DarkAge/Private/AI",
            "DarkAge/Private/UI",
            "DarkAge/Private/Data"
        });

        // Public includes
        PublicIncludePaths.AddRange(new string[]
        {
            "DarkAge/Public",
            "DarkAge/Public/Core",
            "DarkAge/Public/Components",
            "DarkAge/Public/Characters",
            "DarkAge/Public/AI",
            "DarkAge/Public/UI",
            "DarkAge/Public/Data"
        });

        // Enable C++20 features (required by UE5.6 headers)
        CppStandard = CppStandardVersion.Cpp20;

        // Sanitizer settings are not available in UE5.6
        // These would be configured through project settings if needed
    }
}