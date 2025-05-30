// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BPI_PortalJumper.h"
#include "NiagaraChain.h"
#include "GameFramework/Actor.h"
#include "GrabbingObject.generated.h"

UCLASS()
class A_AFRIDI_SHAHID_API AGrabbingObject : public AActor, public INiagaraChain, public IBPI_PortalJumper
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrabbingObject();

	virtual void PerformNiagaraChainSphereTrace_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "Laser")
	void ActivateLaserBeam();

	UFUNCTION(BlueprintCallable, Category = "Laser")
	void DeactivateLaserBeam();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	class UNiagaraSystem* LightningEffect;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	bool bIsLaserActive;
	class UNiagaraComponent* NiagaraComp;

	void PerformLineTraceAndNiagaraUpdate();

	class AGrabbingObject* LastHitGrabbingObject;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Override the interface function
	virtual void JumpPortal_Implementation(const FVector& TargetLocation, const FRotator& TargetRotation) override;

	void ShowHideUI(bool Show);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowHideUIinBP(bool Show);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	float LaserBeamEnd = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	USoundBase* LaserActivateSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	USoundBase* LaserDeactivateSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
	bool ParticipateInLaser;

	bool bPerformLineTrace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
	UStaticMeshComponent* Top;

	FORCEINLINE UStaticMeshComponent* GetItemMesh() const { return ItemMesh; }
};