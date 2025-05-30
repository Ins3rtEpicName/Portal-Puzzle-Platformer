#pragma once

#include "CoreMinimal.h"
#include "NiagaraChain.h"
#include "GameFramework/Actor.h"
#include "Turret.generated.h"

class AGrabbingObject;

UENUM(BlueprintType)
enum class ETurretMode : uint8
{
    Static UMETA(DisplayName = "Static"),
    Rotating UMETA(DisplayName = "Rotating")
};

UCLASS()
class A_AFRIDI_SHAHID_API ATurret : public AActor, public INiagaraChain
{
    GENERATED_BODY()

public:
    ATurret();

    void ShowHideUI(bool Show);

    UFUNCTION(BlueprintImplementableEvent)
    void ShowHideUIinBP(bool Show);

    virtual void PerformNiagaraChainSphereTrace_Implementation() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    ETurretMode TurretMode = ETurretMode::Static;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    class UNiagaraSystem* LightningEffect;

    class UNiagaraComponent* NiagaraComp;
    AGrabbingObject* LastHitGrabbingObject;

protected:
    virtual void BeginPlay() override;
    void RotateTurret(float DeltaTime);

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    float LaserBeamEnd = 5000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    float Damage = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
    UStaticMeshComponent* ItemMesh2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
    UStaticMeshComponent* Top;
};