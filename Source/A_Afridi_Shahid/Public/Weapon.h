// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class A_AFRIDI_SHAHID_API AWeapon : public AActor
{
    GENERATED_BODY()
    
public:    
    // Sets default values for this actor's properties
    AWeapon();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* ItemMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    UParticleSystem* MuzzleFlash;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    float AutoFireRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    int32 Ammo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    int32 AmmoCapacity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
    int32 MagazineAmmo;

public:
    FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
    FORCEINLINE UParticleSystem* GetMuzzleFlash() const { return MuzzleFlash; }
    FORCEINLINE float GetAutoFireRate() const { return AutoFireRate; }

    FORCEINLINE int32 GetAmmo() const { return Ammo; }
    FORCEINLINE void SetAmmo(int32 NewAmmo) { Ammo = NewAmmo; }
    FORCEINLINE float GetAmmoCapacity() const { return AmmoCapacity; }
    void DecreaseAmmo();

    FORCEINLINE int32 GetMagazineAmmo() const { return MagazineAmmo; }
    FORCEINLINE void SetMagazineAmmo(int32 NewMagazineAmmo) { MagazineAmmo += NewMagazineAmmo; }

    void RefillAmmo();
};
