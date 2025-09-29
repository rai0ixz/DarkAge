#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Core/PoliticalSystem.h"
#include "Data/FactionData.h"
#include "PoliticalEventNotifier.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPoliticalNotification, const FString&, NotificationText);

/**
 * Class to handle notifications for political events
 */
UCLASS(BlueprintType, Blueprintable)
class DARKAGE_API UPoliticalEventNotifier : public UObject
{
	GENERATED_BODY()
	
public:
	UPoliticalEventNotifier();
	
	/**
	 * Initialize the notifier
	 */
	UFUNCTION(BlueprintCallable, Category = "Political Notifications")
	void Initialize();
	
	/**
	 * Shutdown the notifier
	 */
	UFUNCTION(BlueprintCallable, Category = "Political Notifications")
	void Shutdown();
	
	/**
	 * Event fired when a political notification is generated
	 */
	UPROPERTY(BlueprintAssignable, Category = "Political Notifications")
	FOnPoliticalNotification OnPoliticalNotification;
	
	/**
	 * Get a singleton instance of the notifier
	 */
	UFUNCTION(BlueprintCallable, Category = "Political Notifications", meta = (WorldContext = "WorldContextObject"))
	static UPoliticalEventNotifier* GetInstance(UObject* WorldContextObject);
	
private:
	/**
	 * Callback for when a political event occurs
	 */
	UFUNCTION()
	void HandlePoliticalEventOccurred(const FPoliticalEvent& Event);
	
	/**
	 * Callback for when a treaty changes
	 */
	UFUNCTION()
	void HandleTreatyChanged(const FPoliticalTreaty& Treaty);
	
	/**
	 * Callback for when faction relationships change
	 */
	UFUNCTION()
	void HandleFactionRelationshipChanged(FName FactionID, FName OtherFactionID, EFactionRelationshipType NewRelationship);
	
	/**
	 * Generate a notification text for a political event
	 */
	FString GenerateEventNotificationText(const FPoliticalEvent& Event, bool bIsNewEvent);
	
	/**
	 * Generate a notification text for a treaty change
	 */
	FString GenerateTreatyNotificationText(const FPoliticalTreaty& Treaty, bool bIsNewTreaty);
	
	/**
	 * Generate a notification text for a faction relationship change
	 */
	FString GenerateRelationshipNotificationText(FName FactionID, FName OtherFactionID, EFactionRelationshipType NewRelationship);
	
	/**
	 * Check if an event is relevant to the player
	 */
	bool IsEventRelevantToPlayer(const FPoliticalEvent& Event);
	
	/**
	 * Check if a treaty is relevant to the player
	 */
	bool IsTreatyRelevantToPlayer(const FPoliticalTreaty& Treaty);
	
	/**
	 * Check if a faction relationship change is relevant to the player
	 */
	bool IsRelationshipRelevantToPlayer(FName FactionID, FName OtherFactionID);
	
	/**
	 * Get a faction's display name
	 */
	FString GetFactionDisplayName(FName FactionID);
	
	/**
	 * Get a region's display name
	 */
	FString GetRegionDisplayName(FName RegionID);
	
	// Singleton instance
	static UPoliticalEventNotifier* Instance;
	
	// Reference to the political system
	UPROPERTY()
	UPoliticalSystem* PoliticalSystem;
	
	// Reference to the faction manager
	UPROPERTY()
	UFactionManagerSubsystem* FactionManager;
};