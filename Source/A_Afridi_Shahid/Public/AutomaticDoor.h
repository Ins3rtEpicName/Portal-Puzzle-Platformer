// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AutomaticDoor.generated.h"

UCLASS()
class A_AFRIDI_SHAHID_API AAutomaticDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAutomaticDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Door state
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door Control")
	bool bCanOpenDoor;


};
