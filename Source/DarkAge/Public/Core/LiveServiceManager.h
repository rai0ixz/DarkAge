#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/IHttpRequest.h"
#include "LiveServiceManager.generated.h"

UENUM(BlueprintType)
enum class ELiveUpdateType : uint8
{
	Content,
	Configuration,
	Hotfix
};

USTRUCT(BlueprintType)
struct FLiveUpdateManifest
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString UpdateId = TEXT("");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString SystemTarget = TEXT("");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString ConfigData = TEXT("");

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Priority = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool RequiresRestart = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FDateTime ExpirationTime;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLiveUpdateReceived, const FLiveUpdateManifest&, UpdateManifest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateApplied, const FString&, UpdateId, bool, bSuccess);

UCLASS()
class DARKAGE_API ULiveServiceManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	ULiveServiceManager();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void StartLiveService(const FString& InServiceEndpoint, const FString& InApiKey);
	void CheckForUpdates();

private:
	void OnUpdateCheckResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void ApplyUpdate(const FLiveUpdateManifest& Manifest);
	void ProcessPendingUpdates();
	bool ValidateUpdate(const FLiveUpdateManifest& UpdateManifest) const;
	void UpdateEcosystemConfig(const FString& ConfigJSON);
	void UpdateCrimeSystemConfig(const FString& ConfigJSON);
	void UpdateSurvivalConfig(const FString& ConfigJSON);
	void UpdateAIConfig(const FString& ConfigJSON);
	void TriggerLiveEvent(const FString& EventType, const FString& EventData);
	FString GetSystemHealthData() const;
	void SendTelemetryData(const FString& TelemetryData);
	void OnTelemetryResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void SendSystemHealthTelemetry();
	void RegisterDebugCommands(class UGameDebugManagerSubsystem* DebugManager);
	FString DebugConnect(const TArray<FString>& Args);
	FString DebugCheckUpdates(const TArray<FString>& Args);
	FString DebugTriggerEvent(const TArray<FString>& Args);
	FString DebugGetHealth(const TArray<FString>& Args);
	void UpdateDataTable(const FString& TableName, const FString& TableData);
	void ReloadAllDataTables();
	void EndLiveEvent(const FString& EventId);

	FString ServiceEndpoint;
	FString ApiKey;
	bool bIsConnected;
	class FHttpModule* HttpModule;
	FTimerHandle UpdateCheckTimer;
	FTimerHandle TelemetryTimer;
	TArray<FLiveUpdateManifest> PendingUpdates;
	TMap<FString, FDateTime> AppliedUpdates;
	TMap<FString, FString> SystemConfigBackups;

public:
	UPROPERTY(BlueprintAssignable, Category = "Live Service")
	FOnLiveUpdateReceived OnLiveUpdateReceived;

	UPROPERTY(BlueprintAssignable, Category = "Live Service")
	FOnUpdateApplied OnUpdateApplied;
};