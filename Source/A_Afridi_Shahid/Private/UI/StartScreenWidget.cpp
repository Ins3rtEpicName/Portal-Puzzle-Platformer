// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/StartScreenWidget.h"

void UStartScreenWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayAnimation(TextFade, 0, 0, EUMGSequencePlayMode::Forward, 1);
	PlayAnimation(SatelliteMove, 0, 0, EUMGSequencePlayMode::PingPong, 0.2);
}