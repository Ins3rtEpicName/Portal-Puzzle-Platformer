#include "MyBlueprintFunctionLibrary.h"
#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"

int32 UMyBlueprintFunctionLibrary::GetLastMaxPlayerIndex()
{
	return UMyGameInstance::MAX_CONTROLLERS - 1;
}

void UMyBlueprintFunctionLibrary::SetActiveControllerID(const UObject * WorldContextObject, int32 ControllerID)
{
	UMyGameInstance* GameInstanceRef = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if (GameInstanceRef)
	{
		GameInstanceRef->SetActiveControllerID(ControllerID);
	}
}

int32 UMyBlueprintFunctionLibrary::GetActiveControllerID(const UObject* WorldContextObject)
{
	UMyGameInstance* GameInstanceRef = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if (GameInstanceRef)
	{
		return GameInstanceRef->GetActiveControllerID();
	}
	return -1;
}

void UMyBlueprintFunctionLibrary::RemoveAllPlayers(const UObject* WorldContextobject)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(WorldContextobject, APlayerController::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		APlayerController* PC = Cast<APlayerController>(Actor);
		if (PC)
		{
			UGameplayStatics::RemovePlayer(PC, true);
		}
	}
}

APlayerController * UMyBlueprintFunctionLibrary::GetActivePlayerController(const UObject* WorldContextObject)
{
	return UGameplayStatics::GetPlayerControllerFromID(WorldContextObject, GetActiveControllerID(WorldContextObject));
}

ACharacter* UMyBlueprintFunctionLibrary::GetActivePlayerCharacter(const UObject* WorldContextObject)
{
	return GetActivePlayerController(WorldContextObject)->GetCharacter();
}

UUIDataAsset* UMyBlueprintFunctionLibrary::GetUIDataAsset(const UObject* WorldContextObject)
{
	const UMyGameInstance* GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if (!GameInstance) return nullptr;

	return GameInstance->GetUIDataAsset();
}

	
