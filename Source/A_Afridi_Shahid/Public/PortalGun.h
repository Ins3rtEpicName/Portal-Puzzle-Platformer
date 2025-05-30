#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "PortalGun.generated.h"

UCLASS()
class A_AFRIDI_SHAHID_API APortalGun : public AGun
{
    GENERATED_BODY()

public:
    APortalGun();

protected:
    virtual void BeginPlay() override;

};
