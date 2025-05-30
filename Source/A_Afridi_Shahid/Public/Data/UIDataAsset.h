// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UIDataAsset.generated.h"

UENUM()
enum EIconType
{
	Select,
	Back,
	// Add more icon types here
};

/**
 * 
 */
UCLASS()
class A_AFRIDI_SHAHID_API UUIDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Icons", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UTexture2D> SelectIcon;

	UPROPERTY(EditDefaultsOnly, Category = "Icons", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<UTexture2D> BackIcon;

public:
	UFUNCTION(BlueprintCallable)
	UTexture2D* GetIcon(EIconType IconType) const;
};
