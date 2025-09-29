#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/World.h"
#include "BTBlackboard.generated.h"

/**
 * Data types that can be stored in the blackboard
 */
UENUM(BlueprintType)
enum class EBlackboardValueType : uint8
{
    Bool        UMETA(DisplayName = "Boolean"),
    Int         UMETA(DisplayName = "Integer"),
    Float       UMETA(DisplayName = "Float"),
    String      UMETA(DisplayName = "String"),
    Name        UMETA(DisplayName = "Name"),
    Vector      UMETA(DisplayName = "Vector"),
    Object      UMETA(DisplayName = "Object"),
    Class       UMETA(DisplayName = "Class")
};

/**
 * A single value stored in the blackboard
 */
USTRUCT(BlueprintType)
struct DARKAGE_API FBlackboardValue
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
    EBlackboardValueType ValueType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
    bool BoolValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
    int32 IntValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
    float FloatValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
    FString StringValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
    FName NameValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
    FVector VectorValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
    UObject* ObjectValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blackboard")
    UClass* ClassValue;

    FBlackboardValue()
        : ValueType(EBlackboardValueType::Bool)
        , BoolValue(false)
        , IntValue(0)
        , FloatValue(0.0f)
        , StringValue("")
        , NameValue(NAME_None)
        , VectorValue(FVector::ZeroVector)
        , ObjectValue(nullptr)
        , ClassValue(nullptr)
    {
    }

    // Convenience constructors
    FBlackboardValue(bool Value) : FBlackboardValue() { ValueType = EBlackboardValueType::Bool; BoolValue = Value; }
    FBlackboardValue(int32 Value) : FBlackboardValue() { ValueType = EBlackboardValueType::Int; IntValue = Value; }
    FBlackboardValue(float Value) : FBlackboardValue() { ValueType = EBlackboardValueType::Float; FloatValue = Value; }
    FBlackboardValue(const FString& Value) : FBlackboardValue() { ValueType = EBlackboardValueType::String; StringValue = Value; }
    FBlackboardValue(FName Value) : FBlackboardValue() { ValueType = EBlackboardValueType::Name; NameValue = Value; }
    FBlackboardValue(const FVector& Value) : FBlackboardValue() { ValueType = EBlackboardValueType::Vector; VectorValue = Value; }
    FBlackboardValue(UObject* Value) : FBlackboardValue() { ValueType = EBlackboardValueType::Object; ObjectValue = Value; }
    FBlackboardValue(UClass* Value) : FBlackboardValue() { ValueType = EBlackboardValueType::Class; ClassValue = Value; }
};

/**
 * Blackboard - Shared memory system for Behavior Trees
 * 
 * The blackboard stores key-value pairs that can be accessed by any node in the behavior tree.
 * This allows nodes to share information and coordinate their behavior.
 */
UCLASS(BlueprintType, Blueprintable)
class DARKAGE_API UDABTBlackboard : public UObject
{
    GENERATED_BODY()

public:
    UDABTBlackboard();

    // --- Value Setters ---

    UFUNCTION(BlueprintCallable, Category = "Blackboard")
    void SetBool(FName Key, bool Value);

    UFUNCTION(BlueprintCallable, Category = "Blackboard")
    void SetInt(FName Key, int32 Value);

    UFUNCTION(BlueprintCallable, Category = "Blackboard")
    void SetFloat(FName Key, float Value);

    UFUNCTION(BlueprintCallable, Category = "Blackboard")
    void SetString(FName Key, const FString& Value);

    UFUNCTION(BlueprintCallable, Category = "Blackboard")
    void SetName(FName Key, FName Value);

    UFUNCTION(BlueprintCallable, Category = "Blackboard")
    void SetVector(FName Key, const FVector& Value);

    UFUNCTION(BlueprintCallable, Category = "Blackboard")
    void SetObject(FName Key, UObject* Value);

    UFUNCTION(BlueprintCallable, Category = "Blackboard")
    void SetClass(FName Key, UClass* Value);

    // --- Value Getters ---

    UFUNCTION(BlueprintPure, Category = "Blackboard")
    bool GetBool(FName Key, bool DefaultValue = false) const;

    UFUNCTION(BlueprintPure, Category = "Blackboard")
    int32 GetInt(FName Key, int32 DefaultValue = 0) const;

    UFUNCTION(BlueprintPure, Category = "Blackboard")
    float GetFloat(FName Key, float DefaultValue = 0.0f) const;

    UFUNCTION(BlueprintPure, Category = "Blackboard")
    FString GetString(FName Key, const FString& DefaultValue = "") const;

    UFUNCTION(BlueprintPure, Category = "Blackboard")
    FName GetName(FName Key, FName DefaultValue = NAME_None) const;

    UFUNCTION(BlueprintPure, Category = "Blackboard")
    FVector GetVector(FName Key, const FVector& DefaultValue = FVector::ZeroVector) const;

    UFUNCTION(BlueprintPure, Category = "Blackboard")
    UObject* GetObject(FName Key, UObject* DefaultValue = nullptr) const;

    UFUNCTION(BlueprintPure, Category = "Blackboard")
    UClass* GetClass(FName Key, UClass* DefaultValue = nullptr) const;

    // --- Generic Value Management ---

    UFUNCTION(BlueprintCallable, Category = "Blackboard")
    void SetValue(FName Key, const FBlackboardValue& Value);

    UFUNCTION(BlueprintPure, Category = "Blackboard")
    FBlackboardValue GetValue(FName Key) const;

    UFUNCTION(BlueprintPure, Category = "Blackboard")
    bool HasKey(FName Key) const;

    UFUNCTION(BlueprintCallable, Category = "Blackboard")
    void RemoveKey(FName Key);

    UFUNCTION(BlueprintCallable, Category = "Blackboard")
    void ClearAll();

    UFUNCTION(BlueprintPure, Category = "Blackboard")
    TArray<FName> GetAllKeys() const;

    UFUNCTION(BlueprintPure, Category = "Blackboard")
    EBlackboardValueType GetValueType(FName Key) const;

    // --- Convenience Functions ---

    /**
     * Set the owner of this blackboard (usually the AI character)
     */
    UFUNCTION(BlueprintCallable, Category = "Blackboard")
    void SetOwner(AActor* InOwner);

    /**
     * Get the owner of this blackboard
     */
    UFUNCTION(BlueprintPure, Category = "Blackboard")
    AActor* GetOwner() const { return Owner; }

    /**
     * Get the owner as a specific type
     */
    template<typename T>
    T* GetOwnerAs() const
    {
        return Cast<T>(Owner);
    }

    /**
     * Set a target actor (commonly used in AI behaviors)
     */
    UFUNCTION(BlueprintCallable, Category = "Blackboard|Convenience")
    void SetTargetActor(AActor* Target);

    /**
     * Get the target actor
     */
    UFUNCTION(BlueprintPure, Category = "Blackboard|Convenience")
    AActor* GetTargetActor() const;

    /**
     * Set a target location (commonly used in AI behaviors)
     */
    UFUNCTION(BlueprintCallable, Category = "Blackboard|Convenience")
    void SetTargetLocation(const FVector& Location);

    /**
     * Get the target location
     */
    UFUNCTION(BlueprintPure, Category = "Blackboard|Convenience")
    FVector GetTargetLocation() const;

    /**
     * Set the current state (useful for state machines within behavior trees)
     */
    UFUNCTION(BlueprintCallable, Category = "Blackboard|Convenience")
    void SetCurrentState(FName State);

    /**
     * Get the current state
     */
    UFUNCTION(BlueprintPure, Category = "Blackboard|Convenience")
    FName GetCurrentState() const;

    /**
     * Check if the blackboard is in a specific state
     */
    UFUNCTION(BlueprintPure, Category = "Blackboard|Convenience")
    bool IsInState(FName State) const;

    // --- Debug Functions ---

    UFUNCTION(BlueprintCallable, Category = "Blackboard|Debug")
    void PrintAllValues() const;

    UFUNCTION(BlueprintPure, Category = "Blackboard|Debug")
    FString GetDebugString() const;

    // --- Events ---

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBlackboardValueChanged, FName, Key, const FBlackboardValue&, NewValue);
    UPROPERTY(BlueprintAssignable, Category = "Blackboard|Events")
    FOnBlackboardValueChanged OnValueChanged;

protected:
    /**
     * The actual data storage
     */
    UPROPERTY()
    TMap<FName, FBlackboardValue> Values;

    /**
     * The owner of this blackboard (usually the AI character)
     */
    UPROPERTY()
    AActor* Owner;

    /**
     * Broadcast value change event
     */
    void BroadcastValueChanged(FName Key, const FBlackboardValue& NewValue);

    // Common key names for convenience functions
    static const FName TargetActorKey;
    static const FName TargetLocationKey;
    static const FName CurrentStateKey;
};