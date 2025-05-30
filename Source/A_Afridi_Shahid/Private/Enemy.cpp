// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnemyWeapon.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "EnemyController.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ShooterCharacter.h"
#include "Soul.h"
#include "Components/SphereComponent.h"

// Sets default values
AEnemy::AEnemy() :
	Health(100.f),
	MaxHealth(100.f),
	MinSpeed(480.f),
	MaxSpeed(750.f),
	bDying(false),
	SoulSpawnHeight(125.f),
	SelfDamageValue(20.f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRangeSphere"));
	CombatRangeSphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	EquipWeapon(SpawnDefaultWeapon());

	// Randomize enemy max movement speed
	float RandomSpeed = FMath::RandRange(MinSpeed, MaxSpeed);
	GetCharacterMovement()->MaxWalkSpeed = RandomSpeed;

	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatRangeOverlap);
	CombatRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatRangeEndOverlap);

	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(
		GetActorTransform(),
		PatrolPoint);
	const FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(
		GetActorTransform(),
		PatrolPoint2);

	EnemyController = Cast<AEnemyController>(GetController());

	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsVector(
			TEXT("PatrolPoint"),
			WorldPatrolPoint);

		EnemyController->GetBlackboardComponent()->SetValueAsVector(
			TEXT("PatrolPoint2"),
			WorldPatrolPoint2);

		EnemyController->RunBehaviorTree(BehaviorTree);
	}
}

AEnemyWeapon* AEnemy::SpawnDefaultWeapon()
{
	if (DefaultWeaponClass)
	{
		// Spawn the Weapon
		return GetWorld()->SpawnActor<AEnemyWeapon>(DefaultWeaponClass);
	}

	return nullptr;
}

void AEnemy::EquipWeapon(AEnemyWeapon* WeaponToEquip)
{
	if (WeaponToEquip)
	{
		// Get the Hand Socket
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			// Attach the Weapon to the hand socket RightHandSocket
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}

		EquippedWeapon = WeaponToEquip;
	}
}

void AEnemy::ActivateWeaponCollision()
{
	EquippedWeapon->GetCollisionBox()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateWeaponCollision()
{
	EquippedWeapon->GetCollisionBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::CombatRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;
	auto Character = Cast<AShooterCharacter>(OtherActor);

	if (Character)
	{
		bInAttackRange = true;
		if (EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), true);
		}

		if (EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsObject(FName("Target"), Character);
		}
	}
}

void AEnemy::CombatRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr) return;
	auto Character = Cast<AShooterCharacter>(OtherActor);

	if (Character)
	{
		bInAttackRange = false;
		if (EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), false);
		}
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsObject(FName("Target"),
			DamageCauser);
	}
	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		Die();

		// Increment the killedEnemies score if DamageCauser is a valid ShooterCharacter
		AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(DamageCauser);
		if (ShooterCharacter && bIsDead == false)
		{
			bIsDead = true;
			ShooterCharacter->IncrementKilledEnemiesScore();
		}

		return DamageAmount;
	}
	else
	{
		Health -= DamageAmount;

		if (ImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
		}
	}

	if (ImpactPaticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactPaticles, GetActorLocation(), FRotator(0.f), true);
	}

	if (bDying) return DamageAmount;

	return DamageAmount;
}

FName AEnemy::GetRandomAttackSectionName()
{
	FName SectionName;
	const int32 Section{ FMath::RandRange(1, 2) };
	switch (Section)
	{
	case 1:
		SectionName = FName(TEXT("Attack1"));
		break;
	case 2:
		SectionName = FName(TEXT("Attack2"));
		break;
	}
	return SectionName;
}

void AEnemy::PlayAttackMontage()
{
	if (bDying) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		FName SectionName = GetRandomAttackSectionName();
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AEnemy::FinishDeath()
{
	Destroy();
	EquippedWeapon->Destroy();
}

void AEnemy::SpawnSoul()
{
	UWorld* World = GetWorld();
	if (World && SoulClass)
	{
		const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, SoulSpawnHeight);
		ASoul* SpawnedSoul = World->SpawnActor<ASoul>(SoulClass, SpawnLocation, GetActorRotation());
	}
}

void AEnemy::Die()
{
	if (bDying) return;
	bDying = true;

	SpawnSoul();

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
	}

	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("Dead"), true);
		EnemyController->StopMovement();
	}
}