// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/StartGM.h"
#include "HUD/StartHUD.h"
#include "PlayerController/StartPC.h"
#include "MyBlueprintFunctionLibrary.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.h"

AStartGM::AStartGM()
	: Super()
{
	// use our custom C++ Start PlayerController class
	PlayerControllerClass = AStartPC::StaticClass();

	// use our custom C++ Start HUD class
	HUDClass = AStartHUD::StaticClass();
}

void AStartGM::StartPlay()
{
	Super::StartPlay();

	UWorld* currentWorld = GetWorld();
	UGameInstance* gameInstance = currentWorld->GetGameInstance();

	for (int32 playerID = 0; playerID <= UMyBlueprintFunctionLibrary::GetLastMaxPlayerIndex(); ++playerID)
	{
		FString error;
		ULocalPlayer* localPlayer = gameInstance->CreateLocalPlayer(playerID, error, true);
	}

	// Play the game's start screen music
	if (UMyGameInstance* gameInstanceRef = Cast<UMyGameInstance>(gameInstance))
	{
		gameInstanceRef->PlayStartScreenMusic();
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("GameInstance not set!"));
		}
	}

}

