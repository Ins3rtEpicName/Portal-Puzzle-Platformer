// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/GameHUD.h"
#include "Blueprint/UserWidget.h"

void AGameHUD::BeginPlay()
{
	// Call the base class
	Super::BeginPlay();

	if (HUDWidgetClass)
	{
		// Create an instance of the Pause Menu Widget
		HUDWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
		HUDWidget->AddToViewport();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("AGameHUD does not have HUDWidgetClass set !!! "));
	}
}

void AGameHUD::DisplayHUD()
{
	if (HUDWidget)
	{
		HUDWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AGameHUD::HideHUD()
{
	if (HUDWidget)
	{
		HUDWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}