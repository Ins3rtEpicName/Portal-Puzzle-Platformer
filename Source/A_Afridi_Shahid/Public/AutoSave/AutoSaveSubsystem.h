// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "AutoSave/AutoSaveGame.h"
#include "AutoSaveSubsystem.generated.h"
/**
 * 
 */
UCLASS()
class A_AFRIDI_SHAHID_API UAutoSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "AutoSave")
	void IncrementJumpCount();

	UFUNCTION(BlueprintCallable, Category = "AutoSave")
	void IncrementPortalsUsedCount();

	UFUNCTION(BlueprintCallable, Category = "AutoSave")
	void IncrementDeathCount();

	UFUNCTION(BlueprintCallable, Category = "AutoSave")
	void IncrementButtonClickCount();

	UFUNCTION(BlueprintCallable, Category = "AutoSave")
	bool SaveGame();

	UFUNCTION(BlueprintCallable, Category = "AutoSave")
	void LoadGame();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> SaveNotificationWidgetClass;

public:
	UFUNCTION(BlueprintCallable, Category = "AutoSave")
	void IncrementObjectsDestroyedCount();

	UFUNCTION(BlueprintCallable, Category = "AutoSave")
	void IncrementSoulsCollectedCount();

	UFUNCTION(BlueprintCallable, Category = "AutoSave")
	void CheckBestTime(int32 NewSeconds, int32 NewMinutes);

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "AutoSave"))
	int32 ObjectsDestroyedCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "AutoSave"))
	int32 SoulsCollectedCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "AutoSave"))
	int32 JumpCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "AutoSave"))
	int32 PortalsUsedCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "AutoSave"))
	int32 DeathCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "AutoSave"))
	int32 ButtonClickCount;

	// best winning time
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "AutoSave"))
	int32 seconds;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "AutoSave"))
	int32 minutes;

protected:
	UPROPERTY()
	UUserWidget* ActiveSaveNotificationWidget;

	void ShowSaveNotificationWidget();
	void HideSaveNotificationWidget();

	FString SlotName = TEXT("AutoSaveSlot");
	FTimerHandle HideWidgetTimerHandle;
};
