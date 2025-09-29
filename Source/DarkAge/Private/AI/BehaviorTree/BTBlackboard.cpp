#include "AI/BehaviorTree/BTBlackboard.h"
#include "Engine/Engine.h"

// Define static const FName members
const FName UDABTBlackboard::TargetActorKey = FName("TargetActor");
const FName UDABTBlackboard::TargetLocationKey = FName("TargetLocation");
const FName UDABTBlackboard::CurrentStateKey = FName("CurrentState");

UDABTBlackboard::UDABTBlackboard()
{
    // Initialize the blackboard
    Owner = nullptr;
    Values.Empty();
}

void UDABTBlackboard::SetBool(FName Key, bool Value)
{
    FBlackboardValue BBValue(Value);
    Values.Add(Key, BBValue);
    BroadcastValueChanged(Key, BBValue);
}

void UDABTBlackboard::SetInt(FName Key, int32 Value)
{
    FBlackboardValue BBValue(Value);
    Values.Add(Key, BBValue);
    BroadcastValueChanged(Key, BBValue);
}

void UDABTBlackboard::SetFloat(FName Key, float Value)
{
    FBlackboardValue BBValue(Value);
    Values.Add(Key, BBValue);
    BroadcastValueChanged(Key, BBValue);
}

void UDABTBlackboard::SetString(FName Key, const FString& Value)
{
    FBlackboardValue BBValue(Value);
    Values.Add(Key, BBValue);
    BroadcastValueChanged(Key, BBValue);
}

void UDABTBlackboard::SetName(FName Key, FName Value)
{
    FBlackboardValue BBValue(Value);
    Values.Add(Key, BBValue);
    BroadcastValueChanged(Key, BBValue);
}

void UDABTBlackboard::SetVector(FName Key, const FVector& Value)
{
    FBlackboardValue BBValue(Value);
    Values.Add(Key, BBValue);
    BroadcastValueChanged(Key, BBValue);
}

void UDABTBlackboard::SetObject(FName Key, UObject* Value)
{
    FBlackboardValue BBValue(Value);
    Values.Add(Key, BBValue);
    BroadcastValueChanged(Key, BBValue);
}

void UDABTBlackboard::SetClass(FName Key, UClass* Value)
{
    FBlackboardValue BBValue(Value);
    Values.Add(Key, BBValue);
    BroadcastValueChanged(Key, BBValue);
}

bool UDABTBlackboard::GetBool(FName Key, bool DefaultValue) const
{
    if (Values.Contains(Key))
    {
        return Values[Key].BoolValue;
    }
    return DefaultValue;
}

int32 UDABTBlackboard::GetInt(FName Key, int32 DefaultValue) const
{
    if (Values.Contains(Key))
    {
        return Values[Key].IntValue;
    }
    return DefaultValue;
}

float UDABTBlackboard::GetFloat(FName Key, float DefaultValue) const
{
    if (Values.Contains(Key))
    {
        return Values[Key].FloatValue;
    }
    return DefaultValue;
}

FString UDABTBlackboard::GetString(FName Key, const FString& DefaultValue) const
{
    if (Values.Contains(Key))
    {
        return Values[Key].StringValue;
    }
    return DefaultValue;
}

FName UDABTBlackboard::GetName(FName Key, FName DefaultValue) const
{
    if (Values.Contains(Key))
    {
        return Values[Key].NameValue;
    }
    return DefaultValue;
}

FVector UDABTBlackboard::GetVector(FName Key, const FVector& DefaultValue) const
{
    if (Values.Contains(Key))
    {
        return Values[Key].VectorValue;
    }
    return DefaultValue;
}

UObject* UDABTBlackboard::GetObject(FName Key, UObject* DefaultValue) const
{
    if (Values.Contains(Key))
    {
        return Values[Key].ObjectValue;
    }
    return DefaultValue;
}

UClass* UDABTBlackboard::GetClass(FName Key, UClass* DefaultValue) const
{
    if (Values.Contains(Key))
    {
        return Values[Key].ClassValue;
    }
    return DefaultValue;
}

void UDABTBlackboard::SetValue(FName Key, const FBlackboardValue& Value)
{
    Values.Add(Key, Value);
    BroadcastValueChanged(Key, Value);
}

FBlackboardValue UDABTBlackboard::GetValue(FName Key) const
{
    if (Values.Contains(Key))
    {
        return Values[Key];
    }
    return FBlackboardValue();
}

bool UDABTBlackboard::HasKey(FName Key) const
{
    return Values.Contains(Key);
}

void UDABTBlackboard::RemoveKey(FName Key)
{
    Values.Remove(Key);
}

void UDABTBlackboard::ClearAll()
{
    Values.Empty();
}

TArray<FName> UDABTBlackboard::GetAllKeys() const
{
    TArray<FName> Keys;
    Values.GetKeys(Keys);
    return Keys;
}

EBlackboardValueType UDABTBlackboard::GetValueType(FName Key) const
{
    if (Values.Contains(Key))
    {
        return Values[Key].ValueType;
    }
    return EBlackboardValueType::Bool; // Default return type
}

void UDABTBlackboard::SetOwner(AActor* InOwner)
{
    Owner = InOwner;
}

void UDABTBlackboard::SetTargetActor(AActor* Target)
{
    SetObject(TargetActorKey, Target);
}

AActor* UDABTBlackboard::GetTargetActor() const
{
    return Cast<AActor>(GetObject(TargetActorKey));
}

void UDABTBlackboard::SetTargetLocation(const FVector& Location)
{
    SetVector(TargetLocationKey, Location);
}

FVector UDABTBlackboard::GetTargetLocation() const
{
    return GetVector(TargetLocationKey);
}

void UDABTBlackboard::SetCurrentState(FName State)
{
    SetName(CurrentStateKey, State);
}

FName UDABTBlackboard::GetCurrentState() const
{
    return GetName(CurrentStateKey);
}

bool UDABTBlackboard::IsInState(FName State) const
{
    return GetCurrentState() == State;
}

void UDABTBlackboard::PrintAllValues() const
{
    for (const auto& Pair : Values)
    {
        FString ValueStr;
        const FBlackboardValue& Value = Pair.Value;
        
        switch (Value.ValueType)
        {
            case EBlackboardValueType::Bool:
                ValueStr = Value.BoolValue ? TEXT("true") : TEXT("false");
                break;
            case EBlackboardValueType::Int:
                ValueStr = FString::FromInt(Value.IntValue);
                break;
            case EBlackboardValueType::Float:
                ValueStr = FString::Printf(TEXT("%.2f"), Value.FloatValue);
                break;
            case EBlackboardValueType::String:
                ValueStr = Value.StringValue;
                break;
            case EBlackboardValueType::Name:
                ValueStr = Value.NameValue.ToString();
                break;
            case EBlackboardValueType::Vector:
                ValueStr = Value.VectorValue.ToString();
                break;
            case EBlackboardValueType::Object:
                ValueStr = Value.ObjectValue ? Value.ObjectValue->GetName() : TEXT("nullptr");
                break;
            case EBlackboardValueType::Class:
                ValueStr = Value.ClassValue ? Value.ClassValue->GetName() : TEXT("nullptr");
                break;
        }
        
        UE_LOG(LogTemp, Log, TEXT("Blackboard Key: %s = %s"), *Pair.Key.ToString(), *ValueStr);
    }
}

FString UDABTBlackboard::GetDebugString() const
{
    FString DebugStr;
    for (const auto& Pair : Values)
    {
        FString ValueStr;
        const FBlackboardValue& Value = Pair.Value;
        
        switch (Value.ValueType)
        {
            case EBlackboardValueType::Bool:
                ValueStr = Value.BoolValue ? TEXT("true") : TEXT("false");
                break;
            case EBlackboardValueType::Int:
                ValueStr = FString::FromInt(Value.IntValue);
                break;
            case EBlackboardValueType::Float:
                ValueStr = FString::Printf(TEXT("%.2f"), Value.FloatValue);
                break;
            case EBlackboardValueType::String:
                ValueStr = Value.StringValue;
                break;
            case EBlackboardValueType::Name:
                ValueStr = Value.NameValue.ToString();
                break;
            case EBlackboardValueType::Vector:
                ValueStr = Value.VectorValue.ToString();
                break;
            case EBlackboardValueType::Object:
                ValueStr = Value.ObjectValue ? Value.ObjectValue->GetName() : TEXT("nullptr");
                break;
            case EBlackboardValueType::Class:
                ValueStr = Value.ClassValue ? Value.ClassValue->GetName() : TEXT("nullptr");
                break;
        }
        
        DebugStr += FString::Printf(TEXT("%s=%s, "), *Pair.Key.ToString(), *ValueStr);
    }
    
    // Remove trailing comma and space
    if (DebugStr.Len() > 2)
    {
        DebugStr.RemoveAt(DebugStr.Len() - 2);
    }
    
    return DebugStr;
}

void UDABTBlackboard::BroadcastValueChanged(FName Key, const FBlackboardValue& NewValue)
{
    OnValueChanged.Broadcast(Key, NewValue);
}