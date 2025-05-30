// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/UIDataAsset.h"

UTexture2D* UUIDataAsset::GetIcon(EIconType IconType) const
{
	switch (IconType)
	{
	case Select:
		return SelectIcon.LoadSynchronous();
	case Back:
		return BackIcon.LoadSynchronous();
	default:
		return nullptr;
	}
}
