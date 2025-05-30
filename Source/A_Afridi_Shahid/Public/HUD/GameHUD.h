// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HUD/BaseHUD.h"
#include "GameHUD.generated.h"

/**
 * 
 */
UCLASS()
class A_AFRIDI_SHAHID_API AGameHUD : public ABaseHUD
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay();

public:
	UFUNCTION(BlueprintCallable)
	void DisplayHUD();

	UFUNCTION(BlueprintCallable)
	void HideHUD();

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> HUDWidgetClass;

	UPROPERTY()
	class UUserWidget* HUDWidget;

};
