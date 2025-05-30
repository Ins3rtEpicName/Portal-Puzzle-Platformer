// Fill out your copyright notice in the Description page of Project Settings.


#include "LockedColourDoor.h"

// Sets default values
ALockedColourDoor::ALockedColourDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALockedColourDoor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALockedColourDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

