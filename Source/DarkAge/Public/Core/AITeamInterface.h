#pragma once

#include "UObject/Interface.h"
#include "AITeamInterface.generated.h"

UINTERFACE(Blueprintable)
class UAITeamInterface : public UInterface
{
    GENERATED_BODY()
};

class IAITeamInterface
{
    GENERATED_BODY()
public:
    // Returns the team/faction ID for this actor (0 = neutral, 1+ = teams)
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI|Team")
    int32 GetTeamId() const;
};
