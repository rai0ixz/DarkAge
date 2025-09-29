#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/Engine.h"
#include "AdminCommandSubsystem.generated.h"

// Admin command delegate for executing commands
DECLARE_DELEGATE_RetVal_TwoParams(FString, FAdminCommandDelegate, const FString&, const TArray<FString>&);

UENUM(BlueprintType)
enum class EAdminPermissionLevel : uint8
{
    None = 0,
    Moderator = 1,      // Basic moderation commands
    GameMaster = 2,     // Gameplay manipulation
    Admin = 3,          // Server configuration
    SuperAdmin = 4      // Full system access
};

USTRUCT()
struct FAdminSession
{
    GENERATED_BODY()
    
    FString AdminId;
    FString SessionId;
    EAdminPermissionLevel PermissionLevel;
    FDateTime LoginTime;
    FDateTime LastActivity;
    FString IPAddress;
    bool bIsActive;
    
    FAdminSession()
        : PermissionLevel(EAdminPermissionLevel::None)
        , bIsActive(false)
    {}
};

USTRUCT()
struct FAdminCommand
{
    GENERATED_BODY()
    
    FString CommandName;
    FAdminCommandDelegate CommandDelegate;
    EAdminPermissionLevel RequiredPermissionLevel;
    FString Description;
    bool bRequiresConfirmation;
    bool bLogged;
    
    FAdminCommand()
        : RequiredPermissionLevel(EAdminPermissionLevel::Admin)
        , bRequiresConfirmation(false)
        , bLogged(true)
    {}
};


/**
 * Secure admin command system with authentication and authorization
 * - Multi-factor authentication required
 * - Role-based permission system
 * - Session management with timeouts
 * - Complete audit logging
 * - IP whitelisting support
 */
UCLASS()
class DARKAGE_API UAdminCommandSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UAdminCommandSubsystem();

    // USubsystem interface
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /**
     * Authenticate admin user with multi-factor authentication
     * @param AdminId - Admin user identifier
     * @param Password - Encrypted password
     * @param TwoFactorCode - 2FA verification code
     * @param IPAddress - Client IP address
     * @return Session token if successful, empty if failed
     */
    UFUNCTION(CallInEditor)
    FString AuthenticateAdmin(const FString& AdminId, const FString& Password, const FString& TwoFactorCode, const FString& IPAddress);

    /**
     * Execute admin command with permission validation
     * @param SessionToken - Valid admin session token
     * @param CommandLine - Command with arguments
     * @return Command result
     */
    UFUNCTION(CallInEditor)
    FString ExecuteAdminCommand(const FString& SessionToken, const FString& CommandLine);


    /**
     * Logout admin session
     */
    UFUNCTION(CallInEditor)
    void LogoutAdmin(const FString& AdminId);

    /**
     * Check if admin is authenticated
     */
    UFUNCTION(BlueprintPure, Category = "Admin")
    bool IsAdminAuthenticated(const FString& AdminId) const;

    /**
     * Get admin permission level
     */
    UFUNCTION(BlueprintPure, Category = "Admin")
    EAdminPermissionLevel GetAdminPermissionLevel(const FString& AdminId) const;

    /**
     * Register admin command with permission requirements
     */
    void RegisterAdminCommand(const FString& CommandName, const FAdminCommandDelegate& CommandDelegate, 
        const FString& Description = TEXT(""), EAdminPermissionLevel RequiredLevel = EAdminPermissionLevel::Admin);

protected:
    UPROPERTY()
    TMap<FString, FAdminSession> ActiveSessions;
    
    UPROPERTY()
    TMap<FString, FAdminCommand> RegisteredCommands;
    
    // Built-in admin commands
    void RegisterBuiltInCommands();
    FString AdminHelp(const FString& AdminId, const TArray<FString>& Args);
    FString AdminStatus(const FString& AdminId, const TArray<FString>& Args);
    FString AdminKickPlayer(const FString& AdminId, const TArray<FString>& Args);
    
    // Security logging
    void LogToSecurityAudit(const FString& AdminId, const FString& Action, const FString& Details);
    static constexpr float SESSION_TIMEOUT = 1800.0f; // 30 minutes
    static constexpr float CLEANUP_INTERVAL = 300.0f; // 5 minutes
};
