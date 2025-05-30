// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AutoSaveGame.generated.h"

UCLASS()
class A_AFRIDI_SHAHID_API UAutoSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "Save Data")
	int32 JumpCount;

	UPROPERTY(VisibleAnywhere, Category = "Save Data")
	int32 PortalsUsedCount;

	UPROPERTY(VisibleAnywhere, Category = "Save Data")
	int32 DeathCount;

	UPROPERTY(VisibleAnywhere, Category = "Save Data")
	int32 ButtonClickCount;

	UPROPERTY(VisibleAnywhere, Category = "Save Data")
	int32 ObjectsDestroyedCount;

	UPROPERTY(VisibleAnywhere, Category = "Save Data")
	int32 SoulsCollectedCount;

	// Best Winning Time
	UPROPERTY(VisibleAnywhere, Category = "Save Data")
	int32 seconds;
	UPROPERTY(VisibleAnywhere, Category = "Save Data")
	int32 minutes;

	UAutoSaveGame()
		: JumpCount(0), ButtonClickCount(0), ObjectsDestroyedCount(0), SoulsCollectedCount(0), seconds(0), minutes(0), PortalsUsedCount(0), DeathCount(0)
	{}
};
