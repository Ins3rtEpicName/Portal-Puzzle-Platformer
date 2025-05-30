// Fill out your copyright notice in the Description page of Project Settings.


#include "Hazard.h"

#include "ShooterCharacter.h"

// Sets default values
AHazard::AHazard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxComponent = CreateDefaultSubobject<UBoxComponent>("MeshComponent");
}

// Called when the game starts or when spawned
void AHazard::BeginPlay()
{
	Super::BeginPlay();

	this->boxComponent->OnComponentBeginOverlap.AddDynamic(this, &AHazard::OnOverlapBegin);
}

// Called every frame
void AHazard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHazard::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor && OtherActor->ActorHasTag("Player"))
    {
        if (AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor))
        {
            ShooterCharacter->GetHit(100);
        }
    }
}
