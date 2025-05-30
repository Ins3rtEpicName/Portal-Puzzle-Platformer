// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUD/BaseHUD.h"
#include "StartHUD.generated.h"

/**
 * 
 */
UCLASS()
class A_AFRIDI_SHAHID_API AStartHUD : public ABaseHUD
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> StartScreenClass;
};
