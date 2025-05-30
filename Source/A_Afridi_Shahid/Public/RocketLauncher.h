#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "RocketLauncher.generated.h"

UCLASS()
class A_AFRIDI_SHAHID_API ARocketLauncher : public AGun
{
    GENERATED_BODY()

public:
    ARocketLauncher();

protected:
    virtual void BeginPlay() override;
};
