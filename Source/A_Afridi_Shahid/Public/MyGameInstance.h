// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "UI/ControllerDisconnectedWidget.h"
#include "MyGameInstance.generated.h"

UCLASS()
class A_AFRIDI_SHAHID_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

	FTSTicker::FDelegateHandle TickDelegateHandle;
	int32 ActiveControllerId = -1;

public:
	static const int32 MAX_CONTROLLERS = 4;

	virtual void Init() override;
	virtual void Shutdown() override;

	bool Tick(float DeltaSeconds);

	void SetActiveControllerID(int32 ControllerID);
	int32 GetActiveControllerID();

private:
	bool bDidControllerDisconnectPauseGame = false;

	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UControllerDisconnectedWidget> ControllerDisconnectedWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget> SaveNotificationWidgetClass;

	UPROPERTY()
	UControllerDisconnectedWidget* ControllerDisconnectedWidget;

	UFUNCTION()
	void OnControllerChanged(EInputDeviceConnectionState connectionState, FPlatformUserId userID, FInputDeviceId inputDeviceId);

private :
	UPROPERTY(EditAnywhere, Category = "UI", meta = (AllowPrivateAccess = "true"))
	class UUIDataAsset* XboxUIDataAsset;

	UPROPERTY(EditAnywhere, Category = "UI", meta = (AllowPrivateAccess = "true"))
	class UUIDataAsset* PS5UIDataAsset;

	UPROPERTY(EditAnywhere, Category = "UI", meta = (AllowPrivateAccess = "true"))
	class UUIDataAsset* SwitchUIDataAsset;

public:
	UFUNCTION(BlueprintCallable)
	class UUIDataAsset* GetUIDataAsset() const;

public:
	UFUNCTION()
	virtual void BeginLoadingScreen(const FString& MapName);
	UFUNCTION()
	virtual void EndLoadingScreen(UWorld* InLoadedworld);

private:
	UPROPERTY()
	class UAudioComponent* GameplayMusicAudioComponent;

	UPROPERTY()
	class UAudioComponent* MenuMusicAudioComponent;

	UPROPERTY()
	class UAudioComponent* StartScreenMusicAudioComponent;

	UPROPERTY(EditAnywhere, Category = "Music", meta = (AllowPrivateAccess = "true"))
	class USoundBase* GameplayMusic;

	UPROPERTY(EditAnywhere, Category = "Music", meta = (AllowPrivateAccess = "true"))
	class USoundBase* MenuMusic;

	UPROPERTY(EditAnywhere, Category = "Music", meta = (AllowPrivateAccess = "true"))
	class USoundBase* StartScreenMusic;

	// Music fade duration
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Music", meta = (AllowPrivateAccess = "true"))
	float MusicFadeDuration = 1.0f;

public:
	// Play Gameplay music
	UFUNCTION(BlueprintCallable)
	void PlayGameplayMusic();

	// Stop Gameplay music
	UFUNCTION(BlueprintCallable)
	void StopGameplayMusic(bool ForceStop = false);

	// Play Menu music
	UFUNCTION(BlueprintCallable)
	void PlayMenuMusic();

	// Stop Menu music
	UFUNCTION(BlueprintCallable)
	void StopMenuMusic(bool ForceStop = false);

	// Play Start Screen music
	UFUNCTION(BlueprintCallable)
	void PlayStartScreenMusic();

	// Stop Start Screen music
	UFUNCTION(BlueprintCallable)
	void StopStartScreenMusic(bool ForceStop = false);
};


