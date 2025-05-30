// Fill out your copyright notice in the Description page of Project Settings.


#include "ColouredKey.h"

#include "ShooterCharacter.h"

// Sets default values
AColouredKey::AColouredKey()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	boxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
}

// Called when the game starts or when spawned
void AColouredKey::BeginPlay()
{
	Super::BeginPlay();

	this->boxComponent->OnComponentBeginOverlap.AddDynamic(this, &AColouredKey::OnOverlapBegin);
}

// Called every frame
void AColouredKey::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AColouredKey::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		if (AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherActor))
		{
			// add key to players inventory
			ShooterCharacter->colouredKeys.Add(this);
			ShooterCharacter->CPP_ShowUIMessage(TEXT("Key Collected"));
		}
	}
}
