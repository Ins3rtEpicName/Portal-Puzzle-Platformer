// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

UCLASS()
class A_AFRIDI_SHAHID_API AGun : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AGun();

private:

	/** Ammo count for this Weapon */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	int32 Ammo = 30;

	/** Maximum ammo that our weapon can hold */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	int32 MagazineCapacity = 30;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void DecrementAmmo();

public:
	UFUNCTION(BlueprintImplementableEvent)
	void PlayGunfireMontage();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* ItemMesh;

	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
};
