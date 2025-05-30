#include "PlayerController/GamePC.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalFogVolume.h"

void AGamePC::BeginPlay()
{
	Super::BeginPlay();
}

void AGamePC::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetWorld()->GetFirstPlayerController()->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}
