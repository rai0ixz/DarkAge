#include "Components/DialogueComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "BaseClass/DAPlayerCharacter.h"
#include "Data/NPCPersonalityData.h"

UDialogueComponent::UDialogueComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    bIsInDialogue = false;
    DialoguePartner = nullptr;
    CurrentLineIndex = 0;
    DefaultTreeID = TEXT("Default");
}

void UDialogueComponent::BeginPlay()
{
    Super::BeginPlay();

    // Initialize default dialogue trees
    InitializeDefaultTrees();
}

void UDialogueComponent::StartDialogue(AActor* Initiator, const FString& TreeID)
{
    if (bIsInDialogue || !Initiator)
    {
        return;
    }

    DialoguePartner = Initiator;
    CurrentTreeID = TreeID.IsEmpty() ? DefaultTreeID : TreeID;
    CurrentLineIndex = 0;
    bIsInDialogue = true;

    // Get first line
    FDialogueTree* Tree = GetCurrentTree();
    if (Tree && Tree->Lines.Num() > 0)
    {
        SetCurrentLine(Tree->Lines[0]);
    }
    else
    {
        // Generate dynamic greeting if no tree exists
        FDialogueLine Greeting = GenerateGreeting(GetOwner());
        SetCurrentLine(Greeting);
    }

    OnDialogueStarted.Broadcast(Initiator);

    UE_LOG(LogTemp, Log, TEXT("Dialogue started between %s and %s"),
        *GetOwner()->GetName(), *Initiator->GetName());
}

void UDialogueComponent::EndDialogue()
{
    if (!bIsInDialogue)
    {
        return;
    }

    bIsInDialogue = false;
    DialoguePartner = nullptr;
    CurrentLineIndex = 0;

    OnDialogueEnded.Broadcast();

    UE_LOG(LogTemp, Log, TEXT("Dialogue ended for %s"), *GetOwner()->GetName());
}

void UDialogueComponent::SelectResponse(int32 ResponseIndex)
{
    if (!bIsInDialogue || !DialoguePartner)
    {
        return;
    }

    FDialogueTree* Tree = GetCurrentTree();
    if (!Tree || CurrentLineIndex >= Tree->Lines.Num())
    {
        EndDialogue();
        return;
    }

    const FDialogueLine& Line = Tree->Lines[CurrentLineIndex];
    if (ResponseIndex < 0 || ResponseIndex >= Line.Responses.Num())
    {
        EndDialogue();
        return;
    }

    // Process response and move to next line or end dialogue
    FString Response = Line.Responses[ResponseIndex];

    // Check for special responses
    if (Response.Contains(TEXT("[END]")))
    {
        EndDialogue();
        return;
    }

    // Move to next line in tree
    CurrentLineIndex++;
    if (CurrentLineIndex < Tree->Lines.Num())
    {
        SetCurrentLine(Tree->Lines[CurrentLineIndex]);
    }
    else
    {
        // Check conditions for next tree
        FString NextTreeID = GetNextTreeID(Tree->Conditions);
        if (!NextTreeID.IsEmpty() && NextTreeID != CurrentTreeID)
        {
            CurrentTreeID = NextTreeID;
            CurrentLineIndex = 0;
            Tree = GetCurrentTree();
            if (Tree && Tree->Lines.Num() > 0)
            {
                SetCurrentLine(Tree->Lines[0]);
            }
            else
            {
                EndDialogue();
            }
        }
        else
        {
            EndDialogue();
        }
    }
}

void UDialogueComponent::AddDialogueTree(const FDialogueTree& Tree)
{
    DialogueTrees.Add(Tree.TreeID, Tree);
}

void UDialogueComponent::SetDefaultTree(const FString& TreeID)
{
    DefaultTreeID = TreeID;
}

FDialogueLine UDialogueComponent::GenerateGreeting(AActor* Speaker)
{
    FDialogueLine Greeting;
    Greeting.SpeakerName = Speaker ? Speaker->GetName() : TEXT("Unknown");
    Greeting.Text = GetPersonalityGreeting();
    Greeting.Mood = GetPersonalityMood();

    // Add default responses
    Greeting.Responses.Add(TEXT("Hello!"));
    Greeting.Responses.Add(TEXT("What can you tell me?"));
    Greeting.Responses.Add(TEXT("Goodbye. [END]"));

    return Greeting;
}

FDialogueLine UDialogueComponent::GenerateFarewell(AActor* Speaker)
{
    FDialogueLine Farewell;
    Farewell.SpeakerName = Speaker ? Speaker->GetName() : TEXT("Unknown");
    Farewell.Text = TEXT("Safe travels, friend.");
    Farewell.Mood = EDialogueMood::Friendly;

    Farewell.Responses.Add(TEXT("Goodbye. [END]"));

    return Farewell;
}

FDialogueLine UDialogueComponent::GenerateQuestDialogue(const FString& QuestID)
{
    FDialogueLine QuestLine;
    QuestLine.SpeakerName = GetOwner()->GetName();
    QuestLine.Text = FString::Printf(TEXT("I have a task for you: %s. Will you help?"), *QuestID);
    QuestLine.Mood = EDialogueMood::Neutral;

    QuestLine.Responses.Add(TEXT("Yes, I'll help."));
    QuestLine.Responses.Add(TEXT("Tell me more."));
    QuestLine.Responses.Add(TEXT("Not right now. [END]"));

    return QuestLine;
}

void UDialogueComponent::SetCurrentLine(const FDialogueLine& Line)
{
    CurrentLine = Line;
    OnLineChanged.Broadcast(Line);
}

FDialogueTree* UDialogueComponent::GetCurrentTree()
{
    return DialogueTrees.Find(CurrentTreeID);
}

bool UDialogueComponent::CheckConditions(const TMap<FString, FString>& Conditions) const
{
    // Simple condition checking - can be expanded
    for (const auto& Condition : Conditions)
    {
        if (Condition.Key == TEXT("HasQuest"))
        {
            // Check if player has specific quest
            // This would integrate with quest system
            return true; // Placeholder
        }
        else if (Condition.Key == TEXT("FactionReputation"))
        {
            // Check faction reputation
            // This would integrate with faction system
            return true; // Placeholder
        }
    }

    return true;
}

FString UDialogueComponent::GetNextTreeID(const TMap<FString, FString>& Conditions) const
{
    for (const auto& Condition : Conditions)
    {
        if (CheckConditions(TMap<FString, FString>{{Condition.Key, TEXT("")}}))
        {
            return Condition.Value;
        }
    }

    return TEXT("");
}

void UDialogueComponent::InitializeDefaultTrees()
{
    // Create default greeting tree
    FDialogueTree DefaultTree;
    DefaultTree.TreeID = TEXT("Default");

    FDialogueLine GreetingLine;
    GreetingLine.SpeakerName = GetOwner()->GetName();
    GreetingLine.Text = TEXT("Hello there, traveler. What brings you to our humble settlement?");
    GreetingLine.Mood = EDialogueMood::Neutral;
    GreetingLine.Responses.Add(TEXT("I'm just exploring."));
    GreetingLine.Responses.Add(TEXT("I'm looking for work."));
    GreetingLine.Responses.Add(TEXT("I need directions."));
    GreetingLine.Responses.Add(TEXT("Goodbye. [END]"));

    DefaultTree.Lines.Add(GreetingLine);

    DialogueTrees.Add(DefaultTree.TreeID, DefaultTree);
}

FString UDialogueComponent::GetPersonalityGreeting() const
{
    // This would use personality data if available
    TArray<FString> Greetings = {
        TEXT("Greetings, stranger."),
        TEXT("Well met, traveler."),
        TEXT("Hello there!"),
        TEXT("What brings you here?")
    };

    return Greetings[FMath::RandRange(0, Greetings.Num() - 1)];
}

FString UDialogueComponent::GetPersonalityResponse(EDialogueMood Mood) const
{
    // Mood-based responses
    switch (Mood)
    {
    case EDialogueMood::Friendly:
        return TEXT("I'm glad to help!");
    case EDialogueMood::Angry:
        return TEXT("Don't waste my time!");
    case EDialogueMood::Suspicious:
        return TEXT("What do you really want?");
    default:
        return TEXT("What can I do for you?");
    }
}

EDialogueMood UDialogueComponent::GetPersonalityMood() const
{
    // This would be based on personality data
    TArray<EDialogueMood> Moods = {
        EDialogueMood::Neutral,
        EDialogueMood::Friendly,
        EDialogueMood::Neutral,
        EDialogueMood::Suspicious
    };

    return Moods[FMath::RandRange(0, Moods.Num() - 1)];
}