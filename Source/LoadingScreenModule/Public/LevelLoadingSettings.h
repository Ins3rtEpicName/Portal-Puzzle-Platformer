// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LevelLoadingSettings.generated.h"

// UCLASS declaration
UCLASS(Config = Game, DefaultConfig)
class LOADINGSCREENMODULE_API ULevelLoadingSettings : public UObject
{
    GENERATED_BODY()

public:
    // Map of Level names to boolcan for shoming a Loading screen
    UPROPERTY(Config, EditAnywhere, Category = "Loading Screens", meta = (AllowedClasses = "/Script/Engine.World"))
    TArray<FSoftObjectPath> LevelLoadingScreens;
    //TMap<FName, bool> LevelLoadingScreens;
};