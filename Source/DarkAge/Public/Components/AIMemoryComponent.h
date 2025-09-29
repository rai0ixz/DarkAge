#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/GlobalEventBus.h"
#include "AIMemoryComponent.generated.h"

UENUM(BlueprintType)
enum class EMemoryType : uint8
{
    Generic,
    Combat,
    Social,
    Betrayal,
    Kindness,
    Faction,
    Trade,
    Quest
};

USTRUCT(BlueprintType)
struct FAIMemory
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Memory")
    FString MemoryId;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Memory")
    TMap<FName, FString> Context;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Memory")
    float EmotionalImpact = 0.0f; // Positive or negative value
   
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Memory")
    float MemoryStrength = 0.0f;
   
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Memory")
    FDateTime Timestamp;
   
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Memory")
    EMemoryType MemoryType = EMemoryType::Generic;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DARKAGE_API UAIMemoryComponent : public UActorComponent, public IGlobalEventListener
{
    GENERATED_BODY()

public:
    UAIMemoryComponent();

    // IGlobalEventListener interface
    virtual TArray<EGlobalEventType> GetListenedEventTypes() const override;
    virtual int32 GetListenerPriority() const override { return 25; } // Low priority

    UFUNCTION(BlueprintCallable, Category = "AI Memory")
    void AddMemory(const FString& MemoryId, const TMap<FName, FString>& Context, float EmotionalImpact, EMemoryType MemoryType, float InitialStrength);

    UFUNCTION(BlueprintPure, Category = "AI Memory")
    bool GetMemory(const FString& MemoryId, FAIMemory& OutMemory) const;

    UFUNCTION(BlueprintCallable, Category = "AI Memory")
    void UpdateMemoryStrength(const FString& MemoryId, float StrengthChange);

    UFUNCTION(BlueprintPure, Category = "AI Memory")
    const TArray<FAIMemory>& GetMemories() const;
        
    // Query memories by type
    UFUNCTION(BlueprintPure, Category = "AI Memory")
    TArray<FAIMemory> GetMemoriesByType(EMemoryType MemoryType) const;
    
    // Query memories by context key/value
    UFUNCTION(BlueprintPure, Category = "AI Memory")
    TArray<FAIMemory> GetMemoriesByContext(FName ContextKey, FString ContextValue) const;

    // --- Faction-Aware Memory Functions ---
    
    /**
     * Get the AI's opinion of a faction based on memories (NOT reputation)
     * This queries the FactionManagerSubsystem for actual reputation data
     * @param FactionID The faction to query
     * @return Opinion modifier based on personal memories (-1.0 to 1.0)
     */
    UFUNCTION(BlueprintPure, Category = "AI Memory|Faction")
    float GetFactionOpinionModifier(FName FactionID) const;
    
    /**
     * Get the actual faction reputation from the authoritative source
     * @param FactionID The faction to query
     * @return Current faction reputation from FactionManagerSubsystem
     */
    UFUNCTION(BlueprintPure, Category = "AI Memory|Faction")
    float GetFactionReputation(FName FactionID) const;
    
    /**
     * Get the AI's effective attitude toward a faction (reputation + personal memories)
     * @param FactionID The faction to query
     * @return Combined attitude value
     */
    UFUNCTION(BlueprintPure, Category = "AI Memory|Faction")
    float GetEffectiveFactionAttitude(FName FactionID) const;
    
    /**
     * Add a faction-related memory
     * @param FactionID The faction this memory relates to
     * @param EventDescription Description of what happened
     * @param EmotionalImpact How this affected the AI's opinion
     * @param MemoryStrength How strong this memory is
     */
    UFUNCTION(BlueprintCallable, Category = "AI Memory|Faction")
    void AddFactionMemory(FName FactionID, const FString& EventDescription, float EmotionalImpact, float MemoryStrength = 1.0f);
    
    /**
     * Get all memories related to a specific faction
     * @param FactionID The faction to query memories for
     * @return Array of faction-related memories
     */
    UFUNCTION(BlueprintPure, Category = "AI Memory|Faction")
    TArray<FAIMemory> GetFactionMemories(FName FactionID) const;
    
    /**
     * Check if the AI remembers a specific player action
     * @param PlayerID The player to check
     * @param ActionType The type of action to look for
     * @param MaxAge Maximum age of memory in game time (0 = any age)
     * @return True if the AI remembers this type of action from this player
     */
    UFUNCTION(BlueprintPure, Category = "AI Memory|Player")
    bool RemembersPlayerAction(FName PlayerID, const FString& ActionType, float MaxAge = 0.0f) const;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // Handle global events
    UFUNCTION(BlueprintImplementableEvent, Category = "Global Events")
    void OnGlobalEventReceived(const FGlobalEvent& Event);

private:
    UPROPERTY()
    TArray<FAIMemory> Memories;

    UPROPERTY(EditAnywhere, Category = "AI Memory")
    float MemoryDecayRate;
    
    // References to other subsystems
    UPROPERTY()
    class UFactionManagerSubsystem* FactionManager;
    
    UPROPERTY()
    class UGlobalEventBus* GlobalEventBus;
    
    // Helper functions
    void RegisterWithEventBus();
    void UnregisterFromEventBus();
    FString GenerateFactionMemoryID(FName FactionID, const FString& EventType) const;
};