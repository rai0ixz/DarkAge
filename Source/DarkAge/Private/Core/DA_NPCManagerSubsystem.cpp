#include "Core/DA_NPCManagerSubsystem.h"
#include "Components/AINeedsPlanningComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "UObject/UObjectIterator.h"

void UDA_NPCManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    for (UAINeedsPlanningComponent* Component : TObjectRange<UAINeedsPlanningComponent>())
    {
        if (Component && IsValid(Component))
        {
            RegisterComponent(Component);
        }
    }
}

void UDA_NPCManagerSubsystem::Deinitialize()
{
    RegisteredComponents.Empty();
    HighFrequencyComponents.Empty();
    MediumFrequencyComponents.Empty();
    LowFrequencyComponents.Empty();
    CachedHostiles.Empty();
    CachedFriendlyNPCs.Empty();
    CachedWaterSources.Empty();
    CachedShelters.Empty();
    Super::Deinitialize();
}

void UDA_NPCManagerSubsystem::Tick(float DeltaTime)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    if (World->GetNetMode() == NM_Client)
    {
        return;
    }

    BucketingTimer += DeltaTime;
    if (BucketingTimer >= 1.0f)
    {
        UpdateBuckets();
        BucketingTimer = 0.0f;
    }

    CacheUpdateTimer += DeltaTime;
    if (CacheUpdateTimer >= 2.0f)
    {
        UpdateWorldCache();
        CacheUpdateTimer = 0.0f;
    }

    UpdateGroupBehaviors(DeltaTime);

    // Time-sliced updates
    for (int32 i = 0; i < 2; ++i)
    {
        if (HighFrequencyComponents.IsValidIndex(HighFrequencyUpdateIndex))
        {
            if (UAINeedsPlanningComponent* C = HighFrequencyComponents[HighFrequencyUpdateIndex])
            {
                C->ManagedTick(DeltaTime);
            }
        }
        HighFrequencyUpdateIndex = (HighFrequencyUpdateIndex + 1) % FMath::Max(1, HighFrequencyComponents.Num());
    }

    if (MediumFrequencyComponents.IsValidIndex(MediumFrequencyUpdateIndex))
    {
        if (UAINeedsPlanningComponent* C = MediumFrequencyComponents[MediumFrequencyUpdateIndex])
        {
            C->ManagedTick(DeltaTime);
        }
    }
    MediumFrequencyUpdateIndex = (MediumFrequencyUpdateIndex + 1) % FMath::Max(1, MediumFrequencyComponents.Num());

    if (GFrameCounter % 2 == 0)
    {
        if (LowFrequencyComponents.IsValidIndex(LowFrequencyUpdateIndex))
        {
            if (UAINeedsPlanningComponent* C = LowFrequencyComponents[LowFrequencyUpdateIndex])
            {
                C->ManagedTick(DeltaTime);
            }
        }
        LowFrequencyUpdateIndex = (LowFrequencyUpdateIndex + 1) % FMath::Max(1, LowFrequencyComponents.Num());
    }
}

void UDA_NPCManagerSubsystem::RegisterComponent(UAINeedsPlanningComponent* Component)
{
    if (Component && IsValid(Component))
    {
        RegisteredComponents.AddUnique(Component);
        LowFrequencyComponents.Add(Component);
    }
}

void UDA_NPCManagerSubsystem::UnregisterComponent(UAINeedsPlanningComponent* Component)
{
    if (Component)
    {
        RegisteredComponents.Remove(Component);
        HighFrequencyComponents.Remove(Component);
        MediumFrequencyComponents.Remove(Component);
        LowFrequencyComponents.Remove(Component);
    }
}

void UDA_NPCManagerSubsystem::UpdateBuckets()
{
    HighFrequencyComponents.Empty();
    MediumFrequencyComponents.Empty();
    LowFrequencyComponents.Empty();

    TArray<FVector> PlayerLocations;
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController* PC = It->Get())
        {
            if (APawn* Pawn = PC->GetPawn())
            {
                PlayerLocations.Add(Pawn->GetActorLocation());
            }
        }
    }

    if (PlayerLocations.Num() == 0)
    {
        LowFrequencyComponents = RegisteredComponents;
        return;
    }

    for (UAINeedsPlanningComponent* Component : RegisteredComponents)
    {
        if (!Component || !IsValid(Component) || !Component->GetOwner())
        {
            continue;
        }

        const FVector NPCLocation = Component->GetOwner()->GetActorLocation();
        float MinDistanceSq = -1.f;
        for (const FVector& PlayerLocation : PlayerLocations)
        {
            const float DistSq = FVector::DistSquared(NPCLocation, PlayerLocation);
            if (MinDistanceSq < 0.f || DistSq < MinDistanceSq)
            {
                MinDistanceSq = DistSq;
            }
        }

        const float HighFreqRadiusSq = FMath::Square(3000.f);
        const float MediumFreqRadiusSq = FMath::Square(8000.f);

        if (MinDistanceSq <= HighFreqRadiusSq)
        {
            HighFrequencyComponents.Add(Component);
        }
        else if (MinDistanceSq <= MediumFreqRadiusSq)
        {
            MediumFrequencyComponents.Add(Component);
        }
        else
        {
            LowFrequencyComponents.Add(Component);
        }
    }
}

void UDA_NPCManagerSubsystem::UpdateWorldCache()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    CachedHostiles.Empty();
    CachedFriendlyNPCs.Empty();
    CachedWaterSources.Empty();
    CachedShelters.Empty();

    for (TActorIterator<AActor> It(World); It; ++It)
    {
        AActor* Actor = *It;
        if (!Actor) continue;
        const FString Name = Actor->GetName();
        if (Name.Contains(TEXT("Wolf")) || Name.Contains(TEXT("Bandit")) || Name.Contains(TEXT("Hostile")))
        {
            CachedHostiles.Add(Actor);
            continue;
        }
        if (Name.Contains(TEXT("NPC")) || Name.Contains(TEXT("Miller")) || Name.Contains(TEXT("Landowner")) || Name.Contains(TEXT("TavernKeeper")))
        {
            CachedFriendlyNPCs.Add(Actor);
            continue;
        }
        if (Name.Contains(TEXT("Well")) || Name.Contains(TEXT("River")) || Name.Contains(TEXT("Water")))
        {
            CachedWaterSources.Add(Actor);
            continue;
        }
        if (Name.Contains(TEXT("House")) || Name.Contains(TEXT("Inn")) || Name.Contains(TEXT("Shelter")))
        {
            CachedShelters.Add(Actor);
            continue;
        }
    }
}

TArray<AActor*> UDA_NPCManagerSubsystem::GetNearbyHostiles(const FVector& Location, float Radius) const
{
    TArray<AActor*> Out;
    const float RadiusSq = FMath::Square(Radius);
    for (AActor* Hostile : CachedHostiles)
    {
        if (Hostile && FVector::DistSquared(Location, Hostile->GetActorLocation()) <= RadiusSq)
        {
            Out.Add(Hostile);
        }
    }
    return Out;
}

TArray<AActor*> UDA_NPCManagerSubsystem::GetNearbyFriendlyNPCs(const FVector& Location, float Radius) const
{
    TArray<AActor*> Out;
    const float RadiusSq = FMath::Square(Radius);
    for (AActor* NPC : CachedFriendlyNPCs)
    {
        if (NPC && FVector::DistSquared(Location, NPC->GetActorLocation()) <= RadiusSq)
        {
            Out.Add(NPC);
        }
    }
    return Out;
}

TArray<AActor*> UDA_NPCManagerSubsystem::GetNearbyWaterSources(const FVector& Location, float Radius) const
{
    TArray<AActor*> Out;
    const float RadiusSq = FMath::Square(Radius);
    for (AActor* Water : CachedWaterSources)
    {
        if (Water && FVector::DistSquared(Location, Water->GetActorLocation()) <= RadiusSq)
        {
            Out.Add(Water);
        }
    }
    return Out;
}

TArray<AActor*> UDA_NPCManagerSubsystem::GetNearbyShelters(const FVector& Location, float Radius) const
{
    TArray<AActor*> Out;
    const float RadiusSq = FMath::Square(Radius);
    for (AActor* Shelter : CachedShelters)
    {
        if (Shelter && FVector::DistSquared(Location, Shelter->GetActorLocation()) <= RadiusSq)
        {
            Out.Add(Shelter);
        }
    }
    return Out;
}

void UDA_NPCManagerSubsystem::UpdateGroupBehaviors(float DeltaTime)
{
    GroupFormationTimer += DeltaTime;
    if (GroupFormationTimer >= 30.0f)
    {
        FormGroupsBasedOnContext();
        GroupFormationTimer = 0.0f;
    }
    UpdateSocialRelationships(DeltaTime);
    CoordinateGroupActions();
}

void UDA_NPCManagerSubsystem::FormGroupsBasedOnContext()
{
    NPCGroups.Empty();
    GroupLeaders.Empty();
    GroupObjectives.Empty();

    TArray<FString> GroupTypes = { TEXT("Patrol"), TEXT("Work"), TEXT("Social"), TEXT("Defense"), TEXT("Trade") };
    for (const FString& GroupType : GroupTypes)
    {
        TArray<TObjectPtr<UAINeedsPlanningComponent>> PotentialMembers;
        for (UAINeedsPlanningComponent* NPC : RegisteredComponents)
        {
            if (ShouldNPCJoinGroup(NPC, GroupType))
            {
                PotentialMembers.Add(NPC);
            }
        }
        if (PotentialMembers.Num() >= 2)
        {
            const FString GroupID = FString::Printf(TEXT("%s_Group_%d"), *GroupType, FMath::RandRange(1, 1000));
            NPCGroups.Add(GroupID, PotentialMembers);
            if (PotentialMembers.Num() > 0 && PotentialMembers[0] && PotentialMembers[0]->GetOwner())
            {
                GroupLeaders.Add(GroupID, PotentialMembers[0]->GetOwner()->GetName());
            }
            AssignGroupObjectives();
            GroupObjectives.Add(GroupID, FString::Printf(TEXT("%s_Objective"), *GroupType));
            UE_LOG(LogTemp, Log, TEXT("Formed %s group with %d members"), *GroupType, PotentialMembers.Num());
        }
    }
}

void UDA_NPCManagerSubsystem::AssignGroupObjectives()
{
    // Placeholder: integrate with world/quest systems
}

void UDA_NPCManagerSubsystem::CoordinateGroupActions()
{
    for (const auto& Pair : NPCGroups)
    {
        const FString& GroupID = Pair.Key;
        const TArray<TObjectPtr<UAINeedsPlanningComponent>>& Members = Pair.Value;
        if (Members.Num() == 0) continue;

        const FString Objective = GroupObjectives.FindRef(GroupID);
        if (Objective.Contains(TEXT("Patrol")))
        {
            ExecuteGroupStrategy(GroupID, TEXT("CoordinatedPatrol"));
        }
        else if (Objective.Contains(TEXT("Defense")))
        {
            ExecuteGroupStrategy(GroupID, TEXT("DefensiveFormation"));
        }
        else if (Objective.Contains(TEXT("Work")))
        {
            ExecuteGroupStrategy(GroupID, TEXT("CollaborativeWork"));
        }
        else if (Objective.Contains(TEXT("Social")))
        {
            ExecuteGroupStrategy(GroupID, TEXT("GroupSocializing"));
        }
    }
}

bool UDA_NPCManagerSubsystem::ShouldNPCJoinGroup(UAINeedsPlanningComponent* NPC, const FString& GroupType)
{
    if (!NPC || !NPC->GetOwner()) return false;

    for (const auto& Pair : NPCGroups)
    {
        if (Pair.Value.Contains(NPC))
        {
            return false;
        }
    }

    const FString NPCName = NPC->GetOwner()->GetName();
    if (GroupType == TEXT("Patrol"))
    {
        return NPCName.Contains(TEXT("Guard")) || NPCName.Contains(TEXT("Warrior"));
    }
    else if (GroupType == TEXT("Work"))
    {
        return NPCName.Contains(TEXT("Miller")) || NPCName.Contains(TEXT("Farmer")) || NPCName.Contains(TEXT("Smith"));
    }
    else if (GroupType == TEXT("Social"))
    {
        return GetNearbyAllies(NPC, 500.0f).Num() >= 2;
    }
    else if (GroupType == TEXT("Defense"))
    {
        // Approximate danger by number of cached hostiles
        const float DangerLevel = FMath::Clamp(CachedHostiles.Num() / 10.0f, 0.0f, 1.0f);
        return DangerLevel > 0.5f;
    }
    return false;
}

void UDA_NPCManagerSubsystem::UpdateSocialRelationships(float DeltaTime)
{
    for (UAINeedsPlanningComponent* NPC1 : RegisteredComponents)
    {
        if (!NPC1 || !NPC1->GetOwner()) continue;
        const FString NPC1Name = NPC1->GetOwner()->GetName();

        for (UAINeedsPlanningComponent* NPC2 : RegisteredComponents)
        {
            if (!NPC2 || NPC1 == NPC2 || !NPC2->GetOwner()) continue;
            const FString NPC2Name = NPC2->GetOwner()->GetName();

            const float CurrentBond = SocialRelationships.FindRef(NPC1Name).FindRef(NPC2Name);
            const float TargetBond = CalculateSocialBond(NPC1, NPC2);
            float NewBond = CurrentBond + (TargetBond - CurrentBond) * DeltaTime * 0.1f;

            if (!SocialRelationships.Contains(NPC1Name))
            {
                SocialRelationships.Add(NPC1Name, TMap<FString, float>());
            }
            SocialRelationships[NPC1Name].Add(NPC2Name, NewBond);
        }
    }
}

float UDA_NPCManagerSubsystem::CalculateSocialBond(UAINeedsPlanningComponent* NPC1, UAINeedsPlanningComponent* NPC2)
{
    if (!NPC1 || !NPC2 || !NPC1->GetOwner() || !NPC2->GetOwner()) return 0.0f;

    float Bond = 0.0f;
    const float Distance = FVector::Dist(NPC1->GetOwner()->GetActorLocation(), NPC2->GetOwner()->GetActorLocation());
    if (Distance < 200.0f) Bond += 0.3f;
    else if (Distance < 500.0f) Bond += 0.1f;

    const FString Name1 = NPC1->GetOwner()->GetName();
    const FString Name2 = NPC2->GetOwner()->GetName();
    if ((Name1.Contains(TEXT("Miller")) && Name2.Contains(TEXT("Miller"))) ||
        (Name1.Contains(TEXT("Guard")) && Name2.Contains(TEXT("Guard"))))
    {
        Bond += 0.4f;
    }

    for (const auto& Pair : NPCGroups)
    {
        if (Pair.Value.Contains(NPC1) && Pair.Value.Contains(NPC2))
        {
            Bond += 0.5f;
            break;
        }
    }

    return FMath::Clamp(Bond, -1.0f, 1.0f);
}

void UDA_NPCManagerSubsystem::ExecuteGroupStrategy(const FString& GroupID, const FString& Strategy)
{
    const TArray<TObjectPtr<UAINeedsPlanningComponent>>* Members = NPCGroups.Find(GroupID);
    if (!Members) return;

    if (Strategy == TEXT("CoordinatedPatrol"))
    {
        for (int32 i = 0; i < Members->Num(); ++i)
        {
            if (UAINeedsPlanningComponent* Member = (*Members)[i])
            {
                HandleGroupCommunication(Member, TEXT("PatrolPosition"), 300.0f);
            }
        }
    }
    else if (Strategy == TEXT("DefensiveFormation"))
    {
        FVector Center = FVector::ZeroVector;
        int32 Count = 0;
        for (UAINeedsPlanningComponent* Member : *Members)
        {
            if (Member && Member->GetOwner())
            {
                Center += Member->GetOwner()->GetActorLocation();
                ++Count;
            }
        }
        if (Count > 0) Center /= Count;

        for (int32 i = 0; i < Members->Num(); ++i)
        {
            if (UAINeedsPlanningComponent* Member = (*Members)[i])
            {
                const float Angle = (2.0f * PI * i) / Members->Num();
                const FVector DefensivePos = Center + FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.0f) * 150.0f;
                HandleGroupCommunication(Member, TEXT("DefensivePosition"), 300.0f);
            }
        }
    }
    else if (Strategy == TEXT("CollaborativeWork"))
    {
        for (UAINeedsPlanningComponent* Member : *Members)
        {
            if (Member)
            {
                HandleGroupCommunication(Member, TEXT("WorkTogether"), 200.0f);
            }
        }
    }
    else if (Strategy == TEXT("GroupSocializing"))
    {
        for (UAINeedsPlanningComponent* Member : *Members)
        {
            if (Member)
            {
                HandleGroupCommunication(Member, TEXT("Socialize"), 200.0f);
            }
        }
    }
}

TArray<UAINeedsPlanningComponent*> UDA_NPCManagerSubsystem::GetNearbyAllies(UAINeedsPlanningComponent* NPC, float Radius)
{
    TArray<UAINeedsPlanningComponent*> Out;
    if (!NPC || !NPC->GetOwner()) return Out;
    const FVector Loc = NPC->GetOwner()->GetActorLocation();
    for (UAINeedsPlanningComponent* Other : RegisteredComponents)
    {
        if (Other && Other != NPC && Other->GetOwner())
        {
            if (FVector::Dist(Loc, Other->GetOwner()->GetActorLocation()) <= Radius)
            {
                Out.Add(Other);
            }
        }
    }
    return Out;
}

void UDA_NPCManagerSubsystem::HandleGroupCommunication(UAINeedsPlanningComponent* Sender, const FString& Message, float Radius)
{
    if (!Sender || !Sender->GetOwner()) return;
    TArray<UAINeedsPlanningComponent*> Nearby = GetNearbyAllies(Sender, Radius);
    for (UAINeedsPlanningComponent* Receiver : Nearby)
    {
        if (Receiver && Receiver->GetOwner())
        {
            UE_LOG(LogTemp, Log, TEXT("Group communication: %s -> %s: %s"),
                *Sender->GetOwner()->GetName(),
                *Receiver->GetOwner()->GetName(),
                *Message);
        }
    }
}


// Public debug/tool entry point to refresh world caches safely
void UDA_NPCManagerSubsystem::ForceUpdateWorldCache()
{
    UpdateWorldCache();
}
