// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/GameGM.h"

#include "ShooterCharacter.h"
#include "HUD/GameHUD.h"
#include "PlayerController/GamePC.h"
#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"

AGameGM::AGameGM()
	: Super()
{
	PlayerControllerClass = AGamePC::StaticClass();
	HUDClass = AGameHUD::StaticClass();
	DefaultPawnClass = AShooterCharacter::StaticClass();
}

void AGameGM::StartPlay()
{
	Super::StartPlay();
	UMyGameInstance* GameInstanceRef = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GameInstanceRef)
	{
		if (GameInstanceRef->GetActiveControllerID() > -1)
		{
			APlayerController* PC = UGameplayStatics::CreatePlayer(this, GameInstanceRef->GetActiveControllerID());

			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(1, 5, FColor::Red, TEXT("ControllerID not set!"));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(1, 5, FColor::Red, TEXT("GameInstance not set!"));
	}

	GameInstanceRef->PlayGameplayMusic();
}
