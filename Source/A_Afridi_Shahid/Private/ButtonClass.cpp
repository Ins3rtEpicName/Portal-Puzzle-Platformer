// Fill out your copyright notice in the Description page of Project Settings.


#include "ButtonClass.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "RotatingLaserBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AButtonClass::AButtonClass()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create and attach the Static Mesh
	ButtonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));
	RootComponent = ButtonMesh;

	// Create and attach the Box Collider
	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	BoxCollider->SetupAttachment(ButtonMesh);

	// Bind overlap events
	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &AButtonClass::OnOverlapBegin);
	BoxCollider->OnComponentEndOverlap.AddDynamic(this, &AButtonClass::OnOverlapEnd);
}

// Called when the game starts or when spawned
void AButtonClass::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AButtonClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AButtonClass::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Occupied) return;

	if (OtherActor && OtherActor->ActorHasTag("GrabbingObject"))
	{
		Occupied = true;

		// Enable gravity for the overlapping actor
		UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(OtherActor->GetRootComponent());
		if (PrimitiveComp && PrimitiveComp->IsSimulatingPhysics())
		{
			PrimitiveComp->SetEnableGravity(true);
		}

		// Play the activate sound
		if (LaserActivateSound)
		{
			UGameplayStatics::PlaySound2D(this, LaserActivateSound);
		}

		// Store the reference to the GrabbingObject
		LastOverlappingGrabbingObject = OtherActor;

		// Spawn the RotatingLaserBase if the class is valid
		if (RotatingLaserInstance)
		{
			RotatingLaserInstance->ActivateLaserBeam();
		}
	}
}

// Handle overlap end
void AButtonClass::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->ActorHasTag("GrabbingObject"))
	{
		// Check if the actor ending the overlap is the same one that activated the laser
		if (OtherActor == LastOverlappingGrabbingObject)
		{
			// Disable gravity for the overlapping actor
			UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(OtherActor->GetRootComponent());
			if (PrimitiveComp && PrimitiveComp->IsSimulatingPhysics())
			{
				PrimitiveComp->SetEnableGravity(false);
			}

			// Deactivate the laser beam if the class is valid
			if (RotatingLaserInstance)
			{
				RotatingLaserInstance->DeactivateLaserBeam();
			}

			// Play the deactivate sound
			if (LaserDeactivateSound)
			{
				UGameplayStatics::PlaySound2D(this, LaserDeactivateSound);
			}

			// Reset the occupied flag
			Occupied = false;
		}
	}
}
