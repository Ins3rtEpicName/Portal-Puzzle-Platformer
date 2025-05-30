// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "HitInterface.h"
#include "Particles/ParticleSystem.h"

// Sets default values
AAmmo::AAmmo() :
	AddAmmoAmount(30)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	OverlapSphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AAmmo::BeginPlay()
{
	Super::BeginPlay();
	
	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AAmmo::OnSphereOverlap);
}

void AAmmo::DestroyPickup()
{
	PlaySoundAndParticle();
}

void AAmmo::PlaySoundAndParticle()
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, GetActorLocation());
	}
	if (DestroyParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestroyParticles, GetActorLocation(), FRotator(0.f), true);
	}

	Destroy();
}

void AAmmo::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(OtherActor);
	if (HitInterface)
	{
		HitInterface->AddAmmo(AddAmmoAmount);

		PlaySoundAndParticle();
	}
}

// Called every frame
void AAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

