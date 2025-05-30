// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/BaseGM.h"
#include "GameGM.generated.h"

/**
 * 
 */
UCLASS()
class A_AFRIDI_SHAHID_API AGameGM : public ABaseGM
{
	GENERATED_BODY()

public:
	AGameGM();

	virtual void StartPlay() override;
};
