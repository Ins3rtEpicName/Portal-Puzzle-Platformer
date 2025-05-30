// Fill out your copyright notice in the Description page of Project Settings.


#include "Soul.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "HitInterface.h"
#include "AutoSave/AutoSaveSubsystem.h"

// Sets default values
ASoul::ASoul()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Embers"));
	RootComponent = ItemEffect;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void ASoul::BeginPlay()
{
	Super::BeginPlay();
	
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ASoul::OnSphereOverlap);
}

void ASoul::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IHitInterface* HitInterface = Cast<IHitInterface>(OtherActor);
	if (HitInterface)
	{
		HitInterface->AddSouls();

		PickupSoundAndDestroy();
	}
}

void ASoul::PickupSoundAndDestroy()
{
	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}

	// Increment the count of objects destroyed
	if (UAutoSaveSubsystem* AutoSaveSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UAutoSaveSubsystem>())
	{
		AutoSaveSubsystem->IncrementSoulsCollectedCount();
	}

	Destroy();
}

void ASoul::DestroyPickup()
{
	PickupSoundAndDestroy();
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Black, TEXT("Destroy Souls"));
}


