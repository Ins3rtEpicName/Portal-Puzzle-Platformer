// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyWeapon.h"
#include "Components/BoxComponent.h"
#include "ShooterCharacter.h"

// Sets default values
AEnemyWeapon::AEnemyWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);

	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	WeaponCollisionBox->SetupAttachment(ItemMesh);

	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponCollisionBox->SetCollisionObjectType(ECC_WorldDynamic);
	WeaponCollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponCollisionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

// Called when the game starts or when spawned
void AEnemyWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyWeapon::OnWeaponOverlap);
}

void AEnemyWeapon::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
	if (ShooterCharacter)
	{
		ShooterCharacter->GetHit(20);
	}
}

// Called every frame
void AEnemyWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

