// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnedPortalGun.h"

// Sets default values
ASpawnedPortalGun::ASpawnedPortalGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpawnedPortalGun::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawnedPortalGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

