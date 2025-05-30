// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BPI_PortalJumper.h"
#include "GameFramework/Actor.h"
#include "PortalBulletBlue.generated.h"

class UProjectileMovementComponent;

UCLASS()
class A_AFRIDI_SHAHID_API APortalBulletBlue : public AActor, public IBPI_PortalJumper
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortalBulletBlue();

	virtual void JumpPortal_Implementation(const FVector& TargetLocation, const FRotator& TargetRotation) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnItemMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	float ProjectileHitPortalOffset = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	float ProjectileImpulseLauchOffsetLocation = 200.0f;

	FORCEINLINE UStaticMeshComponent* GetItemMesh() const { return ItemMesh; }
};
