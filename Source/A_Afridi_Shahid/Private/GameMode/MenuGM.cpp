// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/MenuGM.h"
#include "HUD/MenuHUD.h"
#include "PlayerController/MenuPC.h"
#include "MyGameInstance.h"

AMenuGM::AMenuGM()
	: Super()
{
	PlayerControllerClass = AMenuPC::StaticClass();
	HUDClass = AMenuHUD::StaticClass();
}

void AMenuGM::StartPlay()
{
	Super::StartPlay();

	// Play the game's menu music
	UMyGameInstance* gameInstance = Cast<UMyGameInstance>(GetGameInstance());
	if (gameInstance)
	{
		gameInstance->PlayMenuMusic();
	}
}


