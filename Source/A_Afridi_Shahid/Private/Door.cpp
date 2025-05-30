#include "Door.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ADoor::ADoor()
{
	// Enable ticking
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Implement the Niagara chain interface
void ADoor::OpenDoors()
{
	// Trigger the Blueprint-implementable function to open the door
	OpenDoor();
}
