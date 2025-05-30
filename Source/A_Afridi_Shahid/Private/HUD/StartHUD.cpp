// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/StartHUD.h"
#include "Blueprint/UserWidget.h"

void AStartHUD::BeginPlay()
{
	Super::BeginPlay();

	ensure(StartScreenClass != nullptr);

	if (StartScreenClass)
	{
		UUserWidget* StartScreen = CreateWidget<UUserWidget>(GetWorld(), StartScreenClass);
		StartScreen->AddToViewport();
	}
}
