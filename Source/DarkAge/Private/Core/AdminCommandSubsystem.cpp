#include "Core/AdminCommandSubsystem.h"
#include "Core/SecurityAuditSubsystem.h"
#include "Engine/Engine.h"
#include "Logging/LogMacros.h"
#include "Misc/SecureHash.h"
#include "Engine/GameInstance.h"

UAdminCommandSubsystem::UAdminCommandSubsystem()
{
}

void UAdminCommandSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UE_LOG(LogTemp, Log, TEXT("AdminCommandSubsystem: Initialized - Enterprise Admin System Active"));
    RegisterBuiltInCommands();
}

void UAdminCommandSubsystem::Deinitialize()
{
    RegisteredCommands.Empty();
    ActiveSessions.Empty();
    Super::Deinitialize();
}

void UAdminCommandSubsystem::RegisterAdminCommand(const FString& CommandName, const FAdminCommandDelegate& CommandDelegate, const FString& Description, EAdminPermissionLevel RequiredLevel)
{
    FAdminCommand Command;
    Command.CommandName = CommandName;
    Command.CommandDelegate = CommandDelegate;
    Command.Description = Description;
    Command.RequiredPermissionLevel = RequiredLevel;
    
    RegisteredCommands.Add(CommandName, Command);
    UE_LOG(LogTemp, Log, TEXT("AdminCommandSubsystem: Registered admin command: %s"), *CommandName);
}

FString UAdminCommandSubsystem::ExecuteAdminCommand(const FString& AdminId, const FString& CommandLine)
{
    // Check if admin is authenticated
    if (!IsAdminAuthenticated(AdminId))
    {
        LogToSecurityAudit(AdminId, TEXT("Unauthorized admin command attempt"), CommandLine);
        return TEXT("ERROR: Admin not authenticated");
    }
    
    TArray<FString> Tokens;
    CommandLine.ParseIntoArray(Tokens, TEXT(" "), true);
    
    if (Tokens.Num() == 0)
    {
        return TEXT("ERROR: No command specified");
    }
    
    FString CommandName = Tokens[0];
    TArray<FString> Args;
    for (int32 i = 1; i < Tokens.Num(); i++)
    {
        Args.Add(Tokens[i]);
    }
    
    FAdminCommand* Command = RegisteredCommands.Find(CommandName);
    if (!Command)
    {
        LogToSecurityAudit(AdminId, TEXT("Unknown admin command attempted"), CommandLine);
        return FString::Printf(TEXT("ERROR: Unknown admin command: %s"), *CommandName);
    }
    
    // Check permission level
    EAdminPermissionLevel AdminLevel = GetAdminPermissionLevel(AdminId);
    if (AdminLevel < Command->RequiredPermissionLevel)
    {
        LogToSecurityAudit(AdminId, TEXT("Insufficient permissions for admin command"), CommandLine);
        return TEXT("ERROR: Insufficient permissions");
    }
    
    // Execute command
    if (Command->CommandDelegate.IsBound())
    {
        FString Result = Command->CommandDelegate.Execute(AdminId, Args);
        LogToSecurityAudit(AdminId, TEXT("Admin command executed"), FString::Printf(TEXT("%s -> %s"), *CommandLine, *Result));
        return Result;
    }
    
    return TEXT("ERROR: Command not properly bound");
}


void UAdminCommandSubsystem::LogoutAdmin(const FString& AdminId)
{
    if (FAdminSession* Session = ActiveSessions.Find(AdminId))
    {
        Session->bIsActive = false;
        LogToSecurityAudit(AdminId, TEXT("Admin logged out"), TEXT(""));
        UE_LOG(LogTemp, Log, TEXT("AdminCommandSubsystem: Admin %s logged out"), *AdminId);
    }
}

bool UAdminCommandSubsystem::IsAdminAuthenticated(const FString& AdminId) const
{
    const FAdminSession* Session = ActiveSessions.Find(AdminId);
    return Session && Session->bIsActive;
}

EAdminPermissionLevel UAdminCommandSubsystem::GetAdminPermissionLevel(const FString& AdminId) const
{
    // Basic implementation - in production this would be database-driven
    if (AdminId == TEXT("admin"))
    {
        return EAdminPermissionLevel::SuperAdmin;
    }
    return EAdminPermissionLevel::Moderator;
}

void UAdminCommandSubsystem::RegisterBuiltInCommands()
{
    RegisterAdminCommand(
        TEXT("Admin.Help"),
        FAdminCommandDelegate::CreateUObject(this, &UAdminCommandSubsystem::AdminHelp),
        TEXT("Show available admin commands"),
        EAdminPermissionLevel::Moderator);
    
    RegisterAdminCommand(
        TEXT("Admin.Status"),
        FAdminCommandDelegate::CreateUObject(this, &UAdminCommandSubsystem::AdminStatus),
        TEXT("Show system status"),
        EAdminPermissionLevel::Moderator);
    
    RegisterAdminCommand(
        TEXT("Admin.KickPlayer"),
        FAdminCommandDelegate::CreateUObject(this, &UAdminCommandSubsystem::AdminKickPlayer),
        TEXT("Kick a player from the server"),
        EAdminPermissionLevel::Admin);
}

FString UAdminCommandSubsystem::AdminHelp(const FString& AdminId, const TArray<FString>& Args)
{
    FString HelpText = TEXT("Available Admin Commands:\n");
    EAdminPermissionLevel AdminLevel = GetAdminPermissionLevel(AdminId);
    
    for (const auto& CommandPair : RegisteredCommands)
    {
        const FAdminCommand& Command = CommandPair.Value;
        if (AdminLevel >= Command.RequiredPermissionLevel)
        {
            HelpText += FString::Printf(TEXT("  %s - %s\n"), *Command.CommandName, *Command.Description);
        }
    }
    
    return HelpText;
}

FString UAdminCommandSubsystem::AdminStatus(const FString& AdminId, const TArray<FString>& Args)
{
    FString StatusText = TEXT("System Status:\n");
    StatusText += FString::Printf(TEXT("  Active Admin Sessions: %d\n"), ActiveSessions.Num());
    StatusText += FString::Printf(TEXT("  Registered Commands: %d\n"), RegisteredCommands.Num());
    StatusText += FString::Printf(TEXT("  System Time: %s\n"), *FDateTime::Now().ToString());
    
    return StatusText;
}

FString UAdminCommandSubsystem::AdminKickPlayer(const FString& AdminId, const TArray<FString>& Args)
{
    if (Args.Num() < 1)
    {
        return TEXT("ERROR: Usage: Admin.KickPlayer <PlayerId> [Reason]");
    }
    
    FString PlayerId = Args[0];
    FString Reason = Args.Num() > 1 ? Args[1] : TEXT("No reason specified");
    
    // In a real implementation, this would actually kick the player
    UE_LOG(LogTemp, Warning, TEXT("AdminCommandSubsystem: Kicking player %s - Reason: %s"), *PlayerId, *Reason);
    
    return FString::Printf(TEXT("Player %s has been kicked. Reason: %s"), *PlayerId, *Reason);
}

void UAdminCommandSubsystem::LogToSecurityAudit(const FString& AdminId, const FString& Action, const FString& Details)
{
    if (USecurityAuditSubsystem* SecurityAudit = GetGameInstance()->GetSubsystem<USecurityAuditSubsystem>())
    {
        SecurityAudit->LogSecurityEvent(ESecurityEventType::AdminAction, ESecuritySeverity::Info, AdminId, Action, TEXT(""), Details);
    }
}

FString UAdminCommandSubsystem::AuthenticateAdmin(const FString& AdminId, const FString& Password, const FString& TwoFactorCode, const FString& IPAddress)
{
    // In a real application, you would look up the admin user from a database.
    // For this example, we'll use a hardcoded admin.
    if (AdminId == TEXT("admin"))
    {
        // In a real application, you would use a salted hash.
        FString PasswordToHash = TEXT("password");
        FString StoredPasswordHash = FMD5::HashAnsiString(*PasswordToHash);

        // Hash the provided password
        FString ProvidedPasswordHash = FMD5::HashAnsiString(*Password);

        if (StoredPasswordHash == ProvidedPasswordHash)
        {
            // In a real application, you would validate the 2FA code with a service like Google Authenticator.
            if (TwoFactorCode == TEXT("123456"))
            {
                FAdminSession NewSession;
                NewSession.AdminId = AdminId;
                NewSession.SessionId = FGuid::NewGuid().ToString();
                NewSession.PermissionLevel = EAdminPermissionLevel::SuperAdmin;
                NewSession.LoginTime = FDateTime::Now();
                NewSession.LastActivity = FDateTime::Now();
                NewSession.IPAddress = IPAddress;
                NewSession.bIsActive = true;
                ActiveSessions.Add(AdminId, NewSession);
                return NewSession.SessionId;
            }
        }
    }
    return TEXT("");
}
