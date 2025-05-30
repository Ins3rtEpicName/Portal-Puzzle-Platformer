// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalBulletBlue.h"
#include "Components/StaticMeshComponent.h"
#include "ShooterCharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APortalBulletBlue::APortalBulletBlue()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->bAutoActivate = false;
	ProjectileMovement->InitialSpeed = 0.0f;
	ProjectileMovement->MaxSpeed = 3000.0f;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
}

void APortalBulletBlue::JumpPortal_Implementation(const FVector& TargetLocation, const FRotator& TargetRotation)
{
	// Calculate the offset
	const float OffsetDistance = ProjectileImpulseLauchOffsetLocation; // Adjust this value as needed
	FVector ForwardDirection = TargetRotation.Vector();
	FVector OffsetLocation = TargetLocation + ForwardDirection * OffsetDistance;

	// Set the new location and rotation with offset
	SetActorLocationAndRotation(OffsetLocation, TargetRotation);

	if (ProjectileMovement)
	{
		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->InitialSpeed = 3000.0f;
		ProjectileMovement->Velocity = ForwardDirection * ProjectileMovement->MaxSpeed;
		ProjectileMovement->Activate();
	}
}

// Called when the game starts or when spawned 
void APortalBulletBlue::BeginPlay()
{
	Super::BeginPlay();

	if (ProjectileMovement)
	{
		ProjectileMovement->StopMovementImmediately();
	}

	ItemMesh->OnComponentHit.AddDynamic(this, &APortalBulletBlue::OnItemMeshHit);
}

// Called every frame
void APortalBulletBlue::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APortalBulletBlue::OnItemMeshHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && OtherActor->ActorHasTag(FName("PortalSurface")))
	{
		if (AShooterCharacter* PlayerCharacter = Cast<AShooterCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn()))
		{
			FVector HitLocationOffset = Hit.Location + Hit.Normal * ProjectileHitPortalOffset; // Example offset
			FRotator NormalRotator = Hit.Normal.Rotation();
			bool bIsOrange = false; // Set this based on your game logic

			PlayerCharacter->SpawnRelocatePortalType(bIsOrange, HitLocationOffset, NormalRotator);

			if (PlayerCharacter->PortalTeleportSound)
			{
				UGameplayStatics::PlaySound2D(this, PlayerCharacter->PortalTeleportSound);
			}

			Destroy();
		}
	}
	else
	{
		Destroy();
	}
}
