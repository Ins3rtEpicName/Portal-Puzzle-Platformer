#include "PlayerController/StartPC.h"
#include "MyBlueprintFunctionLibrary.h"
#include "GameFramework/PlayerInput.h"
#include "Kismet/GameplayStatics.h"

bool AStartPC::InputKey(const FInputKeyParams& Params)
{
	if (Params.Event == EInputEvent::IE_Pressed && Params.IsGamepad())
	{
		UMyBlueprintFunctionLibrary::SetActiveControllerID(GetWorld(),
		GetLocalPlayer()->GetLocalPlayerIndex());

		UMyBlueprintFunctionLibrary::RemoveAllPlayers(GetWorld());

		ensure(!NextLevel.IsNull());
		UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), NextLevel);
	}

	return Super::InputKey(Params);
}
