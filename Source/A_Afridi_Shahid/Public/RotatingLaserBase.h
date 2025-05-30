#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RotatingLaserBase.generated.h"

UCLASS()
class A_AFRIDI_SHAHID_API ARotatingLaserBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARotatingLaserBase();

	class UNiagaraComponent* NiagaraComp;

	void ActivateLaserBeam();
	void DeactivateLaserBeam();
	void PerformLineTraceAndNiagaraUpdate();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	class UNiagaraSystem* LightningEffect;

	bool bIsLaserActive;

	class AGrabbingObject* LastHitGrabbingObject;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float LaserBeamEnd = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	UStaticMeshComponent* ItemMesh2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	UStaticMeshComponent* Top;

	float CurrentRotationSpeed;

	// Rotation speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float RotationSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float MaxRotationSpeed = 800.f;
};
