#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DialogueComponent.generated.h"

UENUM(BlueprintType)
enum class EDialogueMood : uint8
{
    Neutral     UMETA(DisplayName = "Neutral"),
    Friendly    UMETA(DisplayName = "Friendly"),
    Angry       UMETA(DisplayName = "Angry"),
    Suspicious  UMETA(DisplayName = "Suspicious"),
    Fearful     UMETA(DisplayName = "Fearful"),
    Excited     UMETA(DisplayName = "Excited")
};

USTRUCT(BlueprintType)
struct FDialogueLine
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FString SpeakerName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FString Text;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    EDialogueMood Mood;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TArray<FString> Responses;

    FDialogueLine()
    {
        SpeakerName = TEXT("");
        Text = TEXT("");
        Mood = EDialogueMood::Neutral;
    }
};

USTRUCT(BlueprintType)
struct FDialogueTree
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FString TreeID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TArray<FDialogueLine> Lines;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TMap<FString, FString> Conditions; // Key: Condition, Value: NextTreeID

    FDialogueTree()
    {
        TreeID = TEXT("");
    }
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DARKAGE_API UDialogueComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDialogueComponent();

    virtual void BeginPlay() override;

    // Dialogue management
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void StartDialogue(AActor* Initiator, const FString& TreeID = TEXT("Default"));

    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void EndDialogue();

    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void SelectResponse(int32 ResponseIndex);

    UFUNCTION(BlueprintPure, Category = "Dialogue")
    bool IsInDialogue() const { return bIsInDialogue; }

    UFUNCTION(BlueprintPure, Category = "Dialogue")
    const FDialogueLine& GetCurrentLine() const { return CurrentLine; }

    UFUNCTION(BlueprintPure, Category = "Dialogue")
    AActor* GetDialoguePartner() const { return DialoguePartner; }

    // Dialogue tree management
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void AddDialogueTree(const FDialogueTree& Tree);

    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void SetDefaultTree(const FString& TreeID);

    // Dynamic dialogue generation
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    FDialogueLine GenerateGreeting(AActor* Speaker);

    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    FDialogueLine GenerateFarewell(AActor* Speaker);

    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    FDialogueLine GenerateQuestDialogue(const FString& QuestID);

    // Events
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueStarted, AActor*, Partner);
    UPROPERTY(BlueprintAssignable, Category = "Dialogue Events")
    FOnDialogueStarted OnDialogueStarted;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueEnded);
    UPROPERTY(BlueprintAssignable, Category = "Dialogue Events")
    FOnDialogueEnded OnDialogueEnded;

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLineChanged, const FDialogueLine&, NewLine);
    UPROPERTY(BlueprintAssignable, Category = "Dialogue Events")
    FOnLineChanged OnLineChanged;

protected:
    // Dialogue state
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dialogue")
    bool bIsInDialogue;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dialogue")
    AActor* DialoguePartner;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dialogue")
    FDialogueLine CurrentLine;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dialogue")
    FString CurrentTreeID;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dialogue")
    int32 CurrentLineIndex;

    // Dialogue trees
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    TMap<FString, FDialogueTree> DialogueTrees;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FString DefaultTreeID;

    // Dialogue functions
    void SetCurrentLine(const FDialogueLine& Line);
    FDialogueTree* GetCurrentTree();
    bool CheckConditions(const TMap<FString, FString>& Conditions) const;
    FString GetNextTreeID(const TMap<FString, FString>& Conditions) const;
    void InitializeDefaultTrees();

    // Personality-based dialogue
    FString GetPersonalityGreeting() const;
    FString GetPersonalityResponse(EDialogueMood Mood) const;
    EDialogueMood GetPersonalityMood() const;
};