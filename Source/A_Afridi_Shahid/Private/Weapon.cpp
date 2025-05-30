// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"

// Sets default v
AWeapon::AWeapon():
	Ammo(30),
	AmmoCapacity(30),
	MagazineAmmo(30)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWeapon::DecreaseAmmo()
{
	Ammo -= 1;
}

void AWeapon::RefillAmmo()
{
    int AmmoToAdd = AmmoCapacity - Ammo;

    if (AmmoToAdd > MagazineAmmo)
    {
        AmmoToAdd = MagazineAmmo;
		Ammo += AmmoToAdd;
		MagazineAmmo = 0;
    }
	else
	{
		Ammo += AmmoToAdd;
		MagazineAmmo -= AmmoToAdd;
	}
}
