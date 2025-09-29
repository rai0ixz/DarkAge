# CultureSubsystem

**Class:** `UCultureSubsystem`  
**Module:** DarkAge  
**Header:** `Source/DarkAge/Public/Core/CultureSubsystem.h`  
**Implementation:** `Source/DarkAge/Private/Core/CultureSubsystem.cpp`

## Overview

The `UCultureSubsystem` manages cultural identities, traditions, and social behaviors in Dark Age. It provides a dynamic cultural system where NPCs belong to different cultures with unique values, practices, and interaction patterns that affect gameplay, dialogue, and world events.

## Key Features

- **Cultural Identity System**: Dynamic cultural groups with unique characteristics
- **Tradition Management**: Cultural practices affecting NPC behavior and interactions
- **Value Systems**: Core beliefs influencing decision-making and relationships
- **Cultural Exchange**: Trading of ideas, practices, and technologies between cultures
- **Cultural Events**: Festivals, ceremonies, and cultural celebrations
- **Language Barriers**: Communication difficulties between different cultures
- **Cultural Adaptation**: NPCs adapting to new cultural environments over time

## Core Functionality

### Culture Management
```cpp
// Culture registration and lookup
UFUNCTION(BlueprintCallable, Category = "Culture")
void RegisterCulture(const FCultureData& CultureData);

UFUNCTION(BlueprintPure, Category = "Culture")
FCultureData GetCultureData(FName CultureName) const;

UFUNCTION(BlueprintPure, Category = "Culture")
TArray<FName> GetAllCultureNames() const;

// NPC culture assignment
UFUNCTION(BlueprintCallable, Category = "Culture")
void AssignCultureToNPC(AActor* NPC, FName CultureName);

UFUNCTION(BlueprintPure, Category = "Culture")
FName GetNPCCulture(AActor* NPC) const;

// Cultural compatibility
UFUNCTION(BlueprintPure, Category = "Culture")
float GetCulturalCompatibility(FName Culture1, FName Culture2) const;
```

### Cultural Values
```cpp
// Value system management
UFUNCTION(BlueprintPure, Category = "Cultural Values")
float GetCulturalValue(FName CultureName, ECulturalValue ValueType) const;

UFUNCTION(BlueprintCallable, Category = "Cultural Values")
void ModifyCulturalValue(FName CultureName, ECulturalValue ValueType, float DeltaValue);

// Value-based decision making
UFUNCTION(BlueprintPure, Category = "Cultural Values")
EDecisionPriority GetDecisionPriority(FName CultureName, EDecisionType DecisionType) const;

UFUNCTION(BlueprintPure, Category = "Cultural Values")
float CalculateCulturalApproval(FName CultureName, const FPlayerAction& Action) const;
```

### Cultural Traditions
```cpp
// Tradition management
UFUNCTION(BlueprintCallable, Category = "Traditions")
void AddCulturalTradition(FName CultureName, const FCulturalTradition& Tradition);

UFUNCTION(BlueprintPure, Category = "Traditions")
TArray<FCulturalTradition> GetActiveTraditions(FName CultureName) const;

UFUNCTION(BlueprintCallable, Category = "Traditions")
void TriggerCulturalEvent(FName CultureName, ECulturalEventType EventType);

// Festival and ceremony management
UFUNCTION(BlueprintCallable, Category = "Events")
void StartCulturalFestival(FName CultureName, FName FestivalName);

UFUNCTION(BlueprintPure, Category = "Events")
bool IsCulturalEventActive(FName CultureName, FName EventName) const;
```

## Data Structures

### FCultureData
```cpp
USTRUCT(BlueprintType)
struct FCultureData : public FTableRowBase
{
    GENERATED_BODY()

    // Basic identity
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName CultureName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    // Cultural characteristics
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<ECulturalValue, float> CulturalValues;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FCulturalTradition> Traditions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> Languages;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName PrimaryLanguage;

    // Social structure
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ESocialStructure SocialStructure = ESocialStructure::Egalitarian;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ELeadershipStyle LeadershipStyle = ELeadershipStyle::Council;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<ESocialRole, float> SocialRoleImportance;

    // Economic characteristics
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EEconomicSystem EconomicSystem = EEconomicSystem::Barter;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> PreferredTradeGoods;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TradeOpenness = 0.5f;

    // Relationships with other cultures
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, float> CulturalRelationships;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> AlliedCultures;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> RivalCultures;

    // Appearance and aesthetics
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSoftObjectPtr<USkeletalMesh>> TraditionalClothing;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TSoftObjectPtr<UStaticMesh>> CulturalArchitecture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor CulturalColor = FLinearColor::White;

    // Adaptation and change
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CulturalAdaptability = 0.3f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float TraditionPreservation = 0.7f;
};
```

### ECulturalValue
```cpp
UENUM(BlueprintType)
enum class ECulturalValue : uint8
{
    Individualism,        // Individual vs collective focus (0-1)
    Hierarchy,           // Social stratification acceptance (0-1)
    Tradition,           // Adherence to traditional ways (0-1)
    Innovation,          // Openness to new ideas (0-1)
    Aggression,          // Acceptance of conflict/violence (0-1)
    Cooperation,         // Preference for working together (0-1)
    Materialism,         // Importance of material wealth (0-1)
    Spirituality,        // Religious/spiritual focus (0-1)
    Honor,               // Importance of personal/family honor (0-1)
    Justice,             // Emphasis on fairness and law (0-1)
    Freedom,             // Value of personal liberty (0-1)
    Security,            // Preference for safety and stability (0-1)
    Nature,              // Harmony with natural world (0-1)
    Knowledge,           // Pursuit of learning and wisdom (0-1)
    Family,              // Importance of family bonds (0-1)
    Community            // Focus on community wellbeing (0-1)
};
```

### FCulturalTradition
```cpp
USTRUCT(BlueprintType)
struct FCulturalTradition
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TraditionName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ETraditionType TraditionType;

    // Behavioral effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<EBehaviorModifier, float> BehaviorModifiers;

    // Timing and frequency
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ETraditionTiming TraditionTiming = ETraditionTiming::Annual;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 SeasonalMonth = 1; // For seasonal traditions

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration = 24.0f; // Hours

    // Requirements and restrictions
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> RequiredItems;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MinParticipants = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<ESocialRole> AllowedParticipants;

    // Effects
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<ECulturalValue, float> ValueReinforcement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CommunityBonding = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float CulturalIdentityBonus = 0.05f;

    // Adaptation over time
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ImportanceLevel = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCanAdapt = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bCanDisappear = false;
};
```

### ETraditionType
```cpp
UENUM(BlueprintType)
enum class ETraditionType : uint8
{
    Religious,           // Religious ceremonies and rituals
    Seasonal,            // Harvest festivals, solstice celebrations
    LifeCycle,           // Birth, coming of age, marriage, death
    Social,              // Community gatherings, storytelling
    Economic,            // Market days, trade ceremonies
    Political,           // Leadership selection, law-making
    Artistic,            // Music, dance, craft traditions
    Culinary,            // Special foods, cooking methods
    Educational,         // Knowledge passing, apprenticeships
    Military,            // Warrior traditions, honor codes
    Environmental        // Nature ceremonies, conservation practices
};
```

## Cultural Interaction System

### Inter-Cultural Relations
```cpp
// Relationship management
UFUNCTION(BlueprintCallable, Category = "Relations")
void ModifyCulturalRelationship(FName Culture1, FName Culture2, float DeltaRelation);

UFUNCTION(BlueprintPure, Category = "Relations")
ECulturalRelationship GetCulturalRelationshipType(FName Culture1, FName Culture2) const;

// Cultural exchange
UFUNCTION(BlueprintCallable, Category = "Exchange")
bool AttemptCulturalExchange(FName SourceCulture, FName TargetCulture, ECulturalExchangeType ExchangeType);

UFUNCTION(BlueprintPure, Category = "Exchange")
float CalculateExchangeProbability(FName SourceCulture, FName TargetCulture, ECulturalExchangeType ExchangeType) const;

// Cultural conflict and cooperation
UFUNCTION(BlueprintCallable, Category = "Conflict")
void TriggerCulturalConflict(FName Culture1, FName Culture2, ECulturalConflictType ConflictType);

UFUNCTION(BlueprintCallable, Category = "Cooperation")
bool InitiateCulturalCooperation(FName Culture1, FName Culture2, ECulturalCooperationType CooperationType);
```

### Language System Integration
```cpp
// Language and communication
UFUNCTION(BlueprintPure, Category = "Language")
float GetLanguageComprehension(AActor* Speaker, AActor* Listener) const;

UFUNCTION(BlueprintPure, Category = "Language")
bool CanCommunicateEffectively(AActor* NPC1, AActor* NPC2) const;

// Translation and interpretation
UFUNCTION(BlueprintCallable, Category = "Language")
FText TranslateMessage(FText OriginalMessage, FName SourceLanguage, FName TargetLanguage, float& TranslationQuality);

UFUNCTION(BlueprintPure, Category = "Language")
bool HasTranslator(FName SourceLanguage, FName TargetLanguage, float MinRadius = 1000.0f) const;
```

## Cultural Adaptation System

### NPC Cultural Adaptation
```cpp
// Adaptation mechanics
UFUNCTION(BlueprintCallable, Category = "Adaptation")
void UpdateCulturalAdaptation(AActor* NPC, FName NewCulture, float ExposureTime);

UFUNCTION(BlueprintPure, Category = "Adaptation")
float GetCulturalAdaptationLevel(AActor* NPC, FName TargetCulture) const;

UFUNCTION(BlueprintCallable, Category = "Adaptation")
void ApplyAdaptationEffects(AActor* NPC);

// Cultural identity crisis
UFUNCTION(BlueprintPure, Category = "Adaptation")
bool IsExperiencingIdentityCrisis(AActor* NPC) const;

UFUNCTION(BlueprintCallable, Category = "Adaptation")
void ResolveCulturalIdentity(AActor* NPC, ECulturalResolution Resolution);
```

### Cultural Change Over Time
```cpp
// Cultural evolution
UFUNCTION(BlueprintCallable, Category = "Evolution")
void UpdateCulturalEvolution(FName CultureName, float DeltaTime);

UFUNCTION(BlueprintCallable, Category = "Evolution")
void ApplyExternalInfluence(FName TargetCulture, FName InfluencingCulture, float InfluenceStrength);

// Tradition modification
UFUNCTION(BlueprintCallable, Category = "Evolution")
void ModifyTradition(FName CultureName, FName TraditionName, const FTraditionChange& Change);

UFUNCTION(BlueprintCallable, Category = "Evolution")
void AddNewTradition(FName CultureName, const FCulturalTradition& NewTradition, ECulturalOrigin Origin);
```

## Events and Delegates

### Cultural Events
```cpp
// Culture change events
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCulturalValueChanged, FName, CultureName, ECulturalValue, ValueType, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCulturalEventTriggered, FName, CultureName, ECulturalEventType, EventType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCulturalExchange, FName, SourceCulture, FName, TargetCulture, ECulturalExchangeType, ExchangeType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCulturalConflict, FName, Culture1, FName, Culture2, ECulturalConflictType, ConflictType);

UPROPERTY(BlueprintAssignable, Category = "Events")
FOnCulturalValueChanged OnCulturalValueChanged;

UPROPERTY(BlueprintAssignable, Category = "Events")
FOnCulturalEventTriggered OnCulturalEventTriggered;

UPROPERTY(BlueprintAssignable, Category = "Events")
FOnCulturalExchange OnCulturalExchange;

UPROPERTY(BlueprintAssignable, Category = "Events")
FOnCulturalConflict OnCulturalConflict;
```

## Integration with Other Systems

### Faction System Integration
```cpp
// Cultural influence on factions
UFUNCTION(BlueprintPure, Category = "Integration")
float GetCulturalInfluenceOnFaction(FName CultureName, FName FactionName) const;

UFUNCTION(BlueprintCallable, Category = "Integration")
void ApplyCulturalPressureToFaction(FName FactionName, TArray<FName> InfluencingCultures);
```

### Economic System Integration
```cpp
// Cultural economic preferences
UFUNCTION(BlueprintPure, Category = "Integration")
TArray<FName> GetPreferredTradePartners(FName CultureName) const;

UFUNCTION(BlueprintPure, Category = "Integration")
float GetTradePriceModifier(FName BuyerCulture, FName SellerCulture, FName ItemType) const;
```

### AI Behavior Integration
```cpp
// Cultural AI behavior modification
UFUNCTION(BlueprintCallable, Category = "Integration")
void ApplyCulturalBehaviorModifiers(AActor* NPC);

UFUNCTION(BlueprintPure, Category = "Integration")
float GetCulturalBehaviorModifier(FName CultureName, EBehaviorType BehaviorType) const;
```

## Usage Examples

### Setting Up Cultural Groups
```cpp
// Initialize cultures during game start
void UMyGameInstance::InitializeCultures()
{
    if (UCultureSubsystem* CultureSys = GetSubsystem<UCultureSubsystem>())
    {
        // Load culture data from data table
        if (UDataTable* CultureDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/_DA/Data/Cultures/CultureData")))
        {
            TArray<FName> CultureNames = CultureDataTable->GetRowNames();
            
            for (FName CultureName : CultureNames)
            {
                if (FCultureData* CultureData = CultureDataTable->FindRow<FCultureData>(CultureName, TEXT("")))
                {
                    CultureSys->RegisterCulture(*CultureData);
                }
            }
        }
    }
}
```

### Cultural-Based NPC Behavior
```cpp
// Modify NPC behavior based on culture
void ANPC::UpdateCulturalBehavior()
{
    if (UCultureSubsystem* CultureSys = GetGameInstance()->GetSubsystem<UCultureSubsystem>())
    {
        FName MyCulture = CultureSys->GetNPCCulture(this);
        
        if (!MyCulture.IsNone())
        {
            FCultureData CultureData = CultureSys->GetCultureData(MyCulture);
            
            // Adjust behavior based on cultural values
            float AggressionValue = CultureData.CulturalValues.FindRef(ECulturalValue::Aggression);
            float CooperationValue = CultureData.CulturalValues.FindRef(ECulturalValue::Cooperation);
            
            // Modify AI behavior tree parameters
            if (UBehaviorTreeComponent* BTComp = FindComponentByClass<UBehaviorTreeComponent>())
            {
                BTComp->GetBlackboardComponent()->SetValueAsFloat(TEXT("AggressionLevel"), AggressionValue);
                BTComp->GetBlackboardComponent()->SetValueAsFloat(TEXT("CooperationLevel"), CooperationValue);
            }
        }
    }
}
```

### Cultural Festival Events
```cpp
// Trigger seasonal cultural festivals
void UCultureSubsystem::UpdateSeasonalEvents(ESeason CurrentSeason)
{
    for (auto& CulturePair : RegisteredCultures)
    {
        FCultureData& Culture = CulturePair.Value;
        
        for (const FCulturalTradition& Tradition : Culture.Traditions)
        {
            if (Tradition.TraditionTiming == ETraditionTiming::Seasonal)
            {
                // Check if it's time for this tradition
                if (ShouldTriggerSeasonalTradition(Tradition, CurrentSeason))
                {
                    StartCulturalFestival(Culture.CultureName, Tradition.TraditionName);
                    
                    // Broadcast global event
                    if (UGlobalEventBus* EventBus = UGlobalEventBus::Get(this))
                    {
                        FGlobalEvent Event(EGlobalEventType::CulturalFestivalStarted, EEventPriority::Normal);
                        Event.EventData.Add(TEXT("Culture"), Culture.CultureName.ToString());
                        Event.EventData.Add(TEXT("Festival"), Tradition.TraditionName.ToString());
                        EventBus->BroadcastEvent(Event);
                    }
                }
            }
        }
    }
}
```

## See Also

- [FactionManagerSubsystem](FactionManagerSubsystem.md) - Political integration with cultures
- [EconomySubsystem](EconomySubsystem.md) - Cultural economic preferences
- [NPCEcosystemSubsystem](NPCEcosystemSubsystem.md) - NPC cultural behaviors
- [DialogueComponent](DialogueComponent.md) - Cultural dialogue variations