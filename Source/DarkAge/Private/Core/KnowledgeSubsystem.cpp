#include "Core/KnowledgeSubsystem.h"

void UKnowledgeSubsystem::RecordExperiment(const TArray<FName>& Inputs, FName Process, FName Result, const FString& PlayerID)
{
    if (!HasDiscoveredPrinciple(Inputs, Process))
    {
        FPrinciple NewPrinciple;
        NewPrinciple.Inputs = Inputs;
        NewPrinciple.Process = Process;
        NewPrinciple.Result = Result;
        NewPrinciple.DiscoveredBy = PlayerID;
        DiscoveredPrinciples.Add(NewPrinciple);
        // Broadcast event for UI/feedback
        OnPrincipleDiscovered.Broadcast(NewPrinciple);
    }
}

bool UKnowledgeSubsystem::HasDiscoveredPrinciple(const TArray<FName>& Inputs, FName Process) const
{
    for (const FPrinciple& Principle : DiscoveredPrinciples)
    {
        if (Principle.Inputs == Inputs && Principle.Process == Process)
        {
            return true;
        }
    }
    return false;
}
