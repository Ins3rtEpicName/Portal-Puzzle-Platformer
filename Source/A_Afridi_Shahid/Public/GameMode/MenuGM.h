// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/BaseGM.h"
#include "MenuGM.generated.h"

/**
 * 
 */
UCLASS()
class A_AFRIDI_SHAHID_API AMenuGM : public ABaseGM
{
	GENERATED_BODY()
public:
	AMenuGM();

	virtual void StartPlay() override;
};
