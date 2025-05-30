// Fill out your copyright notice in the Description page of Project Settings.
#include "ShooterCharacter.h"
#include "Blueprint/UserWidget.h"
#include "HUD/GameHUD.h"
#include "GameFramework/ForceFeedbackEffect.h"
#include "AutoSave/AutoSaveSubsystem.h"
#include "AchievementSubSystem.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Gun.h"
#include "BulletHitInterface.h"
#include "Sound/SoundCue.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "GrabbingObject.h"
//#include "K2Node_SpawnActorFromClass.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Portal2.h"
#include "PortalBulletBlue.h"
#include "PortalBulletRed.h"
#include "Turret.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Checkpoint.h"
#include "RocketLauncher.h"
#include "RocketProjectile.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

// Sets default values
AShooterCharacter::AShooterCharacter() :
	// Health properties
	SelfDamageValue(20.f),
	Health(100.f),
	MaxHealth(100.f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetupAttachment(GetMesh());

	// Create a CameraBoom (Spring Arm Component)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetCapsuleComponent());  // Attach to head bone/socket
	SpringArm->TargetArmLength = 0.f; // Set to 0 for first-person view
	SpringArm->bUsePawnControlRotation = true; // Rotate arm based on controller

	// Create a CameraComponent	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	Camera->SetupAttachment(SpringArm);
	Camera->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	Camera->bUsePawnControlRotation = true;

	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
}

void AShooterCharacter::UnPossessed()
{
	// Remove Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(DefaultMappingContext);
		}
	}
}

void AShooterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(NewController))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AShooterCharacter::SpawnedFirstPortalGun()
{
	EquipWeapon(SpawnPortalGunWeapon(), FName("RightHandSocket"));
	CurrentWeaponType = EWeaponType::EWT_PortalGun;
	SwitchTo(true, false);
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentWeaponType = EWeaponType::EWT_Unarmed;

	if (PortalClass)
	{
		AActor* FoundActor = UGameplayStatics::GetActorOfClass(this, PortalClass);
		PortalClassRef = Cast<APortal2>(FoundActor);
	}

	// Check and set respawn location if a checkpoint is active
	if (CheckpointClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;

		// Spawn the checkpoint at the player's initial location
		ActiveCheckpoint = GetWorld()->SpawnActor<ACheckpoint>(CheckpointClass, GetActorLocation(), FRotator::ZeroRotator, SpawnParams);

		if (ActiveCheckpoint)
		{
			ActiveCheckpoint->ActivateCheckpoint();
		}
	}

	// Timer
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AShooterCharacter::CountDown, 1.f, true, 0.0f);
}

AGun* AShooterCharacter::SpawnPortalGunWeapon()
{
	if (PortalGunWeaponClass)
	{
		// Spawn the Weapon
		return GetWorld()->SpawnActor<AGun>(PortalGunWeaponClass);
	}

	return nullptr;
}

AGun* AShooterCharacter::SpawnRocketLauncherWeapon()
{
	if (RocketLauncherWeaponClass)
	{
		// Spawn the Weapon
		return GetWorld()->SpawnActor<AGun>(RocketLauncherWeaponClass);
	}

	return nullptr;
}

void AShooterCharacter::EquipWeapon(AGun* WeaponToEquip, FName SocketName)
{
	if (WeaponToEquip)
	{
		// Get the Hand Socket
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(SocketName);

		if (HandSocket)
		{
			// Attach the Weapon to the hand socket RightHandSocket
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}

		// Set EquippedWeapon to the newly spawned Weapon
		EquippedWeapon = WeaponToEquip;
	}

	// vfx
	FVector StorageSocketLocation = GetMesh()->GetSocketLocation(SocketName);
	StorageSocketLocation.X += 30.0f;
	if (EquipWeaponPickupEffect) 
	{
		FTransform EffectTransform;
		EffectTransform.SetLocation(StorageSocketLocation);
		EffectTransform.SetScale3D(FVector(0.35f, 0.35f, 0.35f)); 
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EquipWeaponPickupEffect, EffectTransform);
	}

	CombatState = ECombatState::ECS_Unoccupied;
}

void AShooterCharacter::WeaponSwitch()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	CombatState = ECombatState::ECS_SwitchWeapon;

	if (WeaponSwitchSound)  // Ensure PickupSound is set in Blueprint or defaults
	{
		UGameplayStatics::PlaySound2D(GetWorld(), WeaponSwitchSound);
	}

	FTimerHandle TimerHandle_WeaponSwitch;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_WeaponSwitch, this, &AShooterCharacter::ShowWeaponAfterSwitch, 0.25f, false);
}

void AShooterCharacter::ShowWeaponAfterSwitch()
{
	// Toggle between Portal Gun and Rocket Launcher
	if (CurrentWeaponType == EWeaponType::EWT_PortalGun)
	{
		EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;

		EquipWeapon(SpawnRocketLauncherWeapon(), FName("RocketHandSocket"));
		CurrentWeaponType = EWeaponType::EWT_RocketLauncher;
	}
	else if (CurrentWeaponType == EWeaponType::EWT_RocketLauncher)
	{
		EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;

		EquipWeapon(SpawnPortalGunWeapon(), FName("RightHandSocket"));
		CurrentWeaponType = EWeaponType::EWT_PortalGun;
	}

	if (CPP_FPS_ModeOn)
	{
		SwitchTo(false, true);
	}
	else
	{
		StoreWeapon();
	}
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	shootingCooldownTimer += DeltaTime;

	if (IsGrabbingObject)
	{
		// Get the camera's location and rotation
		FVector CameraLocation = Camera->GetComponentLocation();

		// Adjust the grabbed object's position to be at the same distance from the camera, but at the screen center
		float ClampedDistance = FMath::Clamp(GrabbedObjectOffset.Size(), ClampMinGrabDistance, ClampMaxGrabDistance);
		FVector TargetLocation = CameraLocation + Camera->GetForwardVector() * ClampedDistance;

		// Update the position of the grabbed object
		PhysicsHandle->SetTargetLocation(TargetLocation);

		// Update the Niagara system to follow the grabbed object
		if (PhysicsHandle->GrabbedComponent && NiagaraComp)
		{
			FVector GrabbedObjectLocation = PhysicsHandle->GrabbedComponent->GetComponentLocation();
			NiagaraComp->SetNiagaraVariableVec3(TEXT("User.End"), GrabbedObjectLocation);

			const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("MuzzleFlash");
			const FTransform SocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());
			FVector PortalGunSocketLocation = SocketTransform.GetLocation();

			NiagaraComp->SetWorldLocation(PortalGunSocketLocation); // Set the Niagara effect position to the offset location
		}
	}

	if (StayStillAchievementUnlocked == false)
	{
		StayStillTimeCounter += DeltaTime;
		if (StayStillTimeCounter > StayStillAchievementTime)
		{
			UnlockAchievement("Still");
			StayStillAchievementUnlocked = true;
		}
	}
}
void AShooterCharacter::StoreWeapon()
{
	if (CurrentWeaponType == EWeaponType::EWT_Unarmed) return;

	FName StorageSocket;

	// Adjust Scale3D
	if (CurrentWeaponType == EWeaponType::EWT_PortalGun)
	{
		StorageSocket = FName("StoragePortalSocket");
		EquippedWeapon->SetActorScale3D(StorePortalGunScale3D);
	}
	else if (CurrentWeaponType == EWeaponType::EWT_RocketLauncher)
	{
		StorageSocket = FName("StorageRocketLauncherSocket");
		EquippedWeapon->SetActorScale3D(StoreRocketLauncherScale3D);
	}

	EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	EquipWeapon(EquippedWeapon, StorageSocket);
}

void AShooterCharacter::RetrieveWeapon()
{
	FName HandSocket;

	if (CurrentWeaponType == EWeaponType::EWT_Unarmed) return;

	// Attach the weapon back to its respective hand socket
	if (CurrentWeaponType == EWeaponType::EWT_PortalGun)
	{
		HandSocket = FName("RightHandSocket");
	}
	else if (CurrentWeaponType == EWeaponType::EWT_RocketLauncher)
	{
		HandSocket = FName("RocketHandSocket");
	}

	EquippedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	EquipWeapon(EquippedWeapon, HandSocket);

	// Reset weapon size to normal
	EquippedWeapon->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));  // Restore to original size
}


void AShooterCharacter::IncreaseGrabDistance()
{
	if (IsGrabbingObject)
	{
		GrabbedObjectOffset = GrabbedObjectOffset.GetSafeNormal() * FMath::Clamp(GrabbedObjectOffset.Size() + GrabDisIncSpeed, ClampMinGrabDistance, ClampMaxGrabDistance);
		FVector CameraLocation = Camera->GetComponentLocation();
		FVector TargetLocation = CameraLocation + Camera->GetForwardVector() * GrabbedObjectOffset.Size();
		PhysicsHandle->SetTargetLocation(TargetLocation);
	}
}

void AShooterCharacter::DecreaseGrabDistance()
{
	if (IsGrabbingObject)
	{
		GrabbedObjectOffset = GrabbedObjectOffset.GetSafeNormal() * FMath::Clamp(GrabbedObjectOffset.Size() - GrabDisIncSpeed, ClampMinGrabDistance, ClampMaxGrabDistance);
		FVector CameraLocation = Camera->GetComponentLocation();
		FVector TargetLocation = CameraLocation + Camera->GetForwardVector() * GrabbedObjectOffset.Size();
		PhysicsHandle->SetTargetLocation(TargetLocation);
	}
}

void AShooterCharacter::JumpPortal_Implementation(const FVector& TargetLocation, const FRotator& TargetRotation)
{
	FVector Velocity = GetVelocity();
	double JumpLength = Velocity.Size();
	SetJumpVelocity(JumpLength);

	SetActorLocationAndRotation(TargetLocation, TargetRotation);

	AController* PlayerController = GetController();
	if (PlayerController)
	{
		// Set the control rotation
		PlayerController->SetControlRotation(TargetRotation);
	}

	FVector ForwardVector = GetActorForwardVector();
	FVector ForwardJumpVector = ForwardVector * GetJumpVelocity();

	GetCharacterMovement()->Velocity = ForwardJumpVector;
}

void AShooterCharacter::LeftFirePressed()
{
	if (CombatState == ECombatState::ECS_GrabbingObject || CPP_FPS_ModeOn == false)
	{
		ReleaseOrDropObject();
		return;
	}

	if (CombatState == ECombatState::ECS_Unoccupied || bPlayerJumped)
	{
		if (shootingCooldownTimer >= 0.5f)
		{
			if (CurrentWeaponType == EWeaponType::EWT_PortalGun)
			{
				SendBullet(false); // Fire portal bullet

				DisplayUICoolDown();
			}
			else if (CurrentWeaponType == EWeaponType::EWT_RocketLauncher)
			{
				FireRocket(); // Fire rocket instead
			}

			PlayGunfireMontage();
			//EquippedWeapon->DecrementAmmo();
			shootingCooldownTimer = 0.0f;
		}
	}
}

void AShooterCharacter::RightFirePressed()
{
	if (CombatState == ECombatState::ECS_GrabbingObject || CPP_FPS_ModeOn == false)
	{
		ReleaseOrDropObject();
		return;
	}

	if (CombatState == ECombatState::ECS_Unoccupied || bPlayerJumped == true)
	{
		if (shootingCooldownTimer >= 0.5f)
		{
			if (CurrentWeaponType == EWeaponType::EWT_PortalGun)
			{
				SendBullet(true); // Fire portal bullet
				PlayGunfireMontage();

				DisplayUICoolDown();
			}
			else if (CurrentWeaponType == EWeaponType::EWT_RocketLauncher)
			{
				//FireRocket(); // Fire rocket instead
			}

			//EquippedWeapon->DecrementAmmo();
			shootingCooldownTimer = 0.0f;
		}
	}
}

void AShooterCharacter::ReleaseOrDropObject()
{
	if (IsGrabbingObject)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("released"));

		// Get the grabbed component
		UPrimitiveComponent* GrabbedComp = PhysicsHandle->GetGrabbedComponent();
		if (GrabbedComp)
		{
			// Get the current velocity of the grabbed object before releasing
			FVector CurrentVelocity = GrabbedComp->GetPhysicsLinearVelocity();

			GrabbedComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
			GrabbedComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		}

		// Hide UI
		if (GrabbingActor || GrabbingTurretActor)
		{
			AActor* GrabbedActor = GrabbedComp->GetOwner();
			GrabbingActor = Cast<AGrabbingObject>(GrabbedActor);
			if (GrabbingActor)
			{
				GrabbingActor->ShowHideUI(false);
				GrabbingActor = nullptr;
			}
			else
			{
				GrabbingTurretActor = Cast<ATurret>(GrabbedActor);
				if (GrabbingTurretActor)
				{
					GrabbingTurretActor->ShowHideUI(false);
					GrabbingTurretActor = nullptr;
				}
			}
		}

		CombatState = ECombatState::ECS_Unoccupied;
		PhysicsHandle->ReleaseComponent();
		IsGrabbingObject = false;

		DeactivateNiagara();

		if (DropSound)
		{
			UGameplayStatics::PlaySound2D(this, DropSound);
		}


		AddAchievementProgress("Boxing Champion", 1);
	}
}

void AShooterCharacter::DeactivateNiagara()
{
	if(NiagaraComp)
	{
		NiagaraComp->Deactivate();
	}
}

AActor* AShooterCharacter::GetBullet(bool bIsOrange)
{
	if (bIsOrange)
	{
		if (BP_PortalBulletRed)
		{
			return GetWorld()->SpawnActor<APortalBulletRed>(BP_PortalBulletRed, FVector::Zero(), FRotator::ZeroRotator);
		}
	}
	else
	{
		if (BP_PortalBulletBlue)
		{
			return GetWorld()->SpawnActor<APortalBulletBlue>(BP_PortalBulletBlue, FVector::Zero(), FRotator::ZeroRotator);
		}
	}

	return nullptr;
}

void AShooterCharacter::FireRocket()
{
	if (!RocketProjectileClass || !EquippedWeapon) return;

	const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetItemMesh()->GetSocketByName("MuzzleFlash");

	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(EquippedWeapon->GetItemMesh());

		// Spawn Rocket Projectile
		ARocketProjectile* Rocket = GetWorld()->SpawnActor<ARocketProjectile>(
			RocketProjectileClass,
			SocketTransform.GetLocation(),
			SocketTransform.GetRotation().Rotator()
		);
	}
}

void AShooterCharacter::SendBullet(bool bIsOrange)
{
	AActor* BulletActor;

	// Send bullet
	const USkeletalMeshSocket* BarrelSocket =
		EquippedWeapon->GetItemMesh()->GetSocketByName("MuzzleFlash");

	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(
			EquippedWeapon->GetItemMesh());

		FHitResult BeamHitResult;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamHitResult);

		if (bBeamEnd)
		{
			// or any object having this interface
			IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(BeamHitResult.GetActor());
			if (BulletHitInterface)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("BulletHitInterface triggered!"));

				BulletHitInterface->BulletHit_Implementation(this);
			}

			// Check if the hit actor is a ChainLightningAbility actor
			else if (INiagaraChain* NiagaraChainInterface = Cast<INiagaraChain>(BeamHitResult.GetActor()))
			{
				// Frr grabbing object else simply niagara chain that will do chain by sphereMultiTrace
				if (BeamHitResult.GetActor()->ActorHasTag("GrabbingObject"))
				{
					// Calculate distance between the player and the object
					FVector PlayerLocation = GetActorLocation();
					FVector ObjectLocation = BeamHitResult.GetActor()->GetActorLocation();
					float Distance = FVector::Dist(PlayerLocation, ObjectLocation);

					// Check if the object is within 300 units
					if (Distance <= GrabLineTraceDistance)
					{
						PhysicsHandle->GrabComponentAtLocationWithRotation(BeamHitResult.GetComponent(), NAME_None, BeamHitResult.GetComponent()->GetRelativeLocation(), FRotator::ZeroRotator);
						IsGrabbingObject = true;

						// Store the grabbed component reference globally
						GrabbedComponent = BeamHitResult.GetComponent();

						//// Hide UI
						//if (GrabbingActor || GrabbingTurretActor)
						//{
						//	GrabbingActor = Cast<AGrabbingObject>(BeamHitResult.GetActor());
						//	if (GrabbingActor)
						//	{
						//		GrabbingActor->ShowHideUI(false);
						//		GrabbingActor = nullptr;
						//	}
						//	else
						//	{
						//		GrabbingTurretActor = Cast<ATurret>(BeamHitResult.GetActor());
						//		if (GrabbingTurretActor)
						//		{
						//			GrabbingTurretActor->ShowHideUI(false);
						//			GrabbingTurretActor = nullptr;
						//		}
						//	}
						//}

						// Show UI
						GrabbingActor = Cast<AGrabbingObject>(BeamHitResult.GetActor());
						if (GrabbingActor)
						{
							GrabbingActor->ShowHideUI(true);
						}
						else
						{
							GrabbingTurretActor = Cast<ATurret>(BeamHitResult.GetActor());
							if (GrabbingTurretActor)
							{
								GrabbingTurretActor->ShowHideUI(true);
							}
						}

						// Store the relative offset between the camera and the grabbed object
						FVector CameraLocation = Camera->GetComponentLocation();
						FVector GrabbedObjectLocation = BeamHitResult.GetComponent()->GetComponentLocation();
						GrabbedObjectOffset = GrabbedObjectLocation - CameraLocation;

						FVector DirectionToCamera = CameraLocation + Camera->GetForwardVector() * GrabbedObjectOffset.Size();
						PhysicsHandle->SetTargetLocation(DirectionToCamera);

						CombatState = ECombatState::ECS_GrabbingObject;

						ActivateChainLightning(NiagaraChainInterface, BeamHitResult.GetActor(), SocketTransform.GetLocation(), BeamHitResult.GetActor()->GetActorLocation());

						/*if (DoOnceGrabbedObjMessage)
						{
							DoOnceGrabbedObjMessage = false;
							CPP_ShowUIMessage(TEXT("Use QER to Control"));
						}*/

						CPP_ShowUIMessage(TEXT("Use QER to Control"));
					}
				}
			}

			// for placing portal
			else
			{
				GetSurfaceType(BeamHitResult);
				
				FVector HitLocationOffset = BeamHitResult.Location + BeamHitResult.Normal;

				FRotator NormalRotator = UKismetMathLibrary::Conv_VectorToRotator(BeamHitResult.Normal);

				BulletActor = GetBullet(bIsOrange);

				if (BulletActor)
				{
					if (UAutoSaveSubsystem* AutoSaveSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UAutoSaveSubsystem>())
					{
						AutoSaveSubsystem->IncrementPortalsUsedCount();
					}

					BulletActor->SetActorLocation(SocketTransform.GetLocation());
					ProjectileActor = BulletActor;

					// We will use these variables to spawn portal after MoveToComponent has finished
					PendingHitLocationOffset = HitLocationOffset;
					PendingNormalRotator = NormalRotator;
					bPendingIsOrange = bIsOrange;

					FLatentActionInfo LatentInfo;
					LatentInfo.CallbackTarget = this; // Ensure 'this' refers to the correct instance
					LatentInfo.ExecutionFunction = FName(TEXT("OnMoveFinished"));
					LatentInfo.UUID = 0; // Unique ID, can be unique for different calls
					LatentInfo.Linkage = 0; // Should generally be set to 0

					UKismetSystemLibrary::MoveComponentTo(BulletActor->GetRootComponent(), HitLocationOffset, FRotator::ZeroRotator, false, true, PortalProjectileSpawnedMoveSpeed, false, EMoveComponentAction::Move, LatentInfo);
				}
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("ProjectileActor not found"));
				}
			}
		}
	}
}

void AShooterCharacter::OnMoveFinished()
{
	if (bPortalSurface)
	{
		bPortalSurface = false;

		if (ProjectileActor)
		{
			ProjectileActor->Destroy();
		}

		if (PortalTeleportSound)
		{
			UGameplayStatics::PlaySound2D(this, PortalTeleportSound);
		}

		SpawnRelocatePortalType(bPendingIsOrange, PendingHitLocationOffset, PendingNormalRotator);
	}
	else
	{
		if (ProjectileActor)
		{
			ProjectileActor->Destroy();
		}
	}
}

void AShooterCharacter::SpawnRelocatePortalType(bool bIsOrange, FVector HitLocationOffset, FRotator NormalRotator)
{
	if (PortalClassRef)
	{
		PortalClassRef->PlacePortal(bIsOrange, HitLocationOffset, NormalRotator);
	}
}

void AShooterCharacter::RotateGrabbedObjectOnXAxis()
{
	if (GrabbedComponent)
	{
		FRotator xRotator = FRotator::ZeroRotator;
		xRotator.Pitch = GrabObjectRotateSpeed;
		GrabbedComponent->AddLocalRotation(xRotator);
	}
}

void AShooterCharacter::RotateGrabbedObjectOnYAxis()
{
	if (GrabbedComponent)
	{
		FRotator yRotator = FRotator::ZeroRotator;
		yRotator.Yaw = GrabObjectRotateSpeed;
		GrabbedComponent->AddLocalRotation(yRotator);
	}
}

void AShooterCharacter::RotateGrabbedObjectOnZAxis()
{
	if (GrabbedComponent)
	{
		FRotator zRotator = FRotator::ZeroRotator;
		zRotator.Roll= GrabObjectRotateSpeed;
		GrabbedComponent->AddLocalRotation(zRotator);
	}
}

void AShooterCharacter::ActivateChainLightning(INiagaraChain* NiagaraChainInterface, AActor* Actor, const FVector& StartLocation, const FVector& EndLocation)
{
	// Spawn lightning effect at the hit location
	if (LightningEffect)
	{
		// Attach Niagara to player with the offset location
		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
			LightningEffect,
			GetMesh(), // Attach to the player's mesh
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::KeepRelativeOffset,
			true
		);

		if (NiagaraComp)
		{
			// Update the end location of the lightning effect
			NiagaraComp->SetNiagaraVariableVec3(TEXT("User.End"), EndLocation);
			NiagaraComp->SetWorldLocation(StartLocation); // Set the Niagara effect position to the offset location
		}
	}
}

bool AShooterCharacter::GetBeamEndLocation(
	const FVector& MuzzleSocketLocation,
	FHitResult& OutHitResult)
{
	FVector OutBeamLocation;
	// Check for crosshair trace hit
	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation);

	if (bCrosshairHit)
	{
		// Tentative beam location - still need to trace from gun
		OutBeamLocation = CrosshairHitResult.Location;
	}
	else // no crosshair trace hit
	{
		// OutBeamLocation is the End location for the line trace
	}

	// Perform a second trace, this time from the gun barrel
	const FVector WeaponTraceStart{ MuzzleSocketLocation };
	const FVector StartToEnd{ OutBeamLocation - WeaponTraceStart };
	const FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd * 1.25f };
	GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		WeaponTraceStart,
		WeaponTraceEnd,
		ECollisionChannel::ECC_Visibility);
	if (!OutHitResult.bBlockingHit) // object between barrel and BeamEndPoint?
	{
		OutHitResult.Location = OutBeamLocation;
		return false;
	}

	return true;
}

bool AShooterCharacter::TraceUnderCrosshairs(
	FHitResult& OutHitResult,
	FVector& OutHitLocation)
{
	// Get Viewport Size
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	// Get screen space location of crosshairs
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	// Get world position and direction of crosshairs
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		// Trace from Crosshair world location outward
		const FVector Start{ CrosshairWorldPosition };
		const FVector End{ Start + CrosshairWorldDirection * LineTraceDistance};
		OutHitLocation = End;

		if(ShowLineTrace)
		{
			DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 5.0f, 0, 0.5);
		}

		GetWorld()->LineTraceSingleByChannel(
			OutHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);
		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////// Input

//void AShooterCharacter::Move(const FInputActionValue& Value)
//{
//	if (MovementVector.Size() > 0.0f)
//	{
//		StayStillTimeCounter = 0.f;
//		UnlockAchievement("Move");
//	}
//}

void AShooterCharacter::PlayForceFeedbackEffect(UForceFeedbackEffect* FeedbackEffect)
{
	if (FeedbackEffect)
	{
		if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
		{
			PlayerController->ClientPlayForceFeedback(FeedbackEffect);
		}
	}
}

//void AShooterCharacter::SendBullet()
//{
//	IExplosiveInterface* ExplosiveInterface = Cast<IExplosiveInterface>(CrosshairHitResult.GetActor());
//	if (ExplosiveInterface)
//	{
//		PlayForceFeedbackEffect(FireForceFeedbackEffect);
//
//		IncrementExplosiveScore();
//	}
//
//	AEnemy* HitEnemy = Cast<AEnemy>(CrosshairHitResult.GetActor());
//	if (HitEnemy)
//	{
//		UnlockAchievement(FName("FirstBlood"));
//	}
//}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Completed, this, &AShooterCharacter::Pause);

		// Jumping
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AShooterCharacter::Jump);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AShooterCharacter::StopJumping);

		// Moving
		//EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Move);

		// Looking
		//EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShooterCharacter::Look);

		// WeaponSwitch
		EnhancedInputComponent->BindAction(SwitchWeaponAction, ETriggerEvent::Started, this, &AShooterCharacter::WeaponSwitch);

		// Sprint
		//EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AShooterCharacter::ShiftButtonPressed);
		//EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AShooterCharacter::ShiftButtonReleased);

		EnhancedInputComponent->BindAction(LeftFireAction, ETriggerEvent::Started, this, &AShooterCharacter::LeftFirePressed);
		EnhancedInputComponent->BindAction(RightFireAction, ETriggerEvent::Started, this, &AShooterCharacter::RightFirePressed);

		EnhancedInputComponent->BindAction(IncreaseGrabDistAction, ETriggerEvent::Triggered, this, &AShooterCharacter::IncreaseGrabDistance);
		EnhancedInputComponent->BindAction(DecreaseGrabDistAction, ETriggerEvent::Triggered, this, &AShooterCharacter::DecreaseGrabDistance);

		// Rotate grabbed object
		EnhancedInputComponent->BindAction(RotateGrabbedObjectXAxis, ETriggerEvent::Triggered, this, &AShooterCharacter::RotateGrabbedObjectOnXAxis);
		EnhancedInputComponent->BindAction(RotateGrabbedObjectYAxis, ETriggerEvent::Triggered, this, &AShooterCharacter::RotateGrabbedObjectOnYAxis);
		EnhancedInputComponent->BindAction(RotateGrabbedObjectZAxis, ETriggerEvent::Triggered, this, &AShooterCharacter::RotateGrabbedObjectOnZAxis);

#if PLATFORM_SWITCH
		EnhancedInputComponent->BindAction(BackActionBottom, ETriggerEvent::Completed, this, &AShooterCharacter::Back);
#else
		EnhancedInputComponent->BindAction(BackActionRight, ETriggerEvent::Completed, this, &AShooterCharacter::Back);
#endif

	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Enhanced Input component not found!"));
		}
	}
}

void AShooterCharacter::ToggleDrone()
{
	//if (CombatState != ECombatState::ECS_Unoccupied) return;

	/*if (Drone)
	{
		GetCharacterMovement()->StopMovementImmediately();
		Controller->Possess(Drone);
	}*/
}

void AShooterCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}

	if (SprintPressed)
	{
		GetCharacterMovement()->MaxWalkSpeed = 900.f;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
	}
}

void AShooterCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AShooterCharacter::ShiftButtonPressed()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	SprintPressed = true;
	CombatState = ECombatState::ECS_Sprint;
}

void AShooterCharacter::ShiftButtonReleased()
{
	if (SprintPressed)
	{
		SprintPressed = false;
		CombatState = ECombatState::ECS_Unoccupied;
	}
}

void AShooterCharacter::Jump()
{
	bPlayerJumped = true;
	ACharacter::Jump();
}

void AShooterCharacter::StopJump()
{
	ACharacter::StopJumping();
}

void AShooterCharacter::PlayGunfireMontage()
{
	// Play fire sound
	if (EquippedWeapon)
	{
		EquippedWeapon->PlayGunfireMontage();
	}
}

void AShooterCharacter::GetHit(int Damage)
{
	if (shootingCooldownTimer >= 0.5f)
	{
		if (Health == 0.f) return;

		if (Health - Damage <= 0.f)
		{
			Health = 0.f;
			Die();
		}
		else
		{
			Health -= Damage;

			if (ImpactSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
			}

			PlayForceFeedbackEffect(HurtForceFeedbackEffect);

			CPP_ShowUIMessage(TEXT("Stay calm man"));
		}

		if (ImpactPaticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactPaticles, GetActorLocation(), FRotator(0.f), true);
		}

		shootingCooldownTimer = 0.0f;
	}
}

void AShooterCharacter::Die()
{
	if (UAutoSaveSubsystem* AutoSaveSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UAutoSaveSubsystem>())
	{
		AutoSaveSubsystem->IncrementDeathCount();
	}

	UnlockAchievement("First Fall");
	// Play sound
	if (DeathSound)
	{
		UGameplayStatics::PlaySound2D(this, DeathSound);
	}

	if (ActiveCheckpoint)
	{
		SetActorLocation(ActiveCheckpoint->GetRespawnLocation());
		Health = MaxHealth; // Reset health
	}
	else
	{
		// Handle case when no checkpoint is available
		UE_LOG(LogTemp, Warning, TEXT("No active checkpoint set!"));
	}

	//DisplayResult();
}

//void AShooterCharacter::AddSouls()
//{
//	IncrementSoulsScore();
//	PlayForceFeedbackEffect(PickupForceFeedbackEffect);
//}
//
//void AShooterCharacter::AddAmmo(int32 AddAmmoAmount)
//{
//	if (EquippedWeapon == nullptr) return;
//
//	EquippedWeapon->SetMagazineAmmo(AddAmmoAmount);
//	PlayForceFeedbackEffect(PickupForceFeedbackEffect);
//}

void AShooterCharacter::IncrementKilledEnemiesScore()
{
	KilledEnemies_Score++;

	if (KilledEnemies_Score == TotalEnemies)
	{
		// Check Best time
		if (UAutoSaveSubsystem* AutoSaveSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UAutoSaveSubsystem>())
		{
			AutoSaveSubsystem->CheckBestTime(Seconds, Minutes);
		}

		if (Health == MaxHealth)
		{
			UnlockAchievement(FName("Perfectionist"));
		}

		DisplayResult();
	}
}

void AShooterCharacter::CountDown()
{
	if (Seconds != 0)
	{
		Seconds = Seconds - 1;
	}
	else
	{
		if (Minutes == 0)
		{
			UnlockAchievement(FName("TimeUp"));

			DisplayResult();
		}
		else
		{
			Minutes = Minutes - 1;
			Seconds = 59;
		}
	}
}

void AShooterCharacter::Pause()
{
	if (!UGameplayStatics::IsGamePaused(GetWorld())) // NOT PAUSED
	{
		if (PauseMenuWidgetClass)
		{
			// Create an instance of the Pause Menu Widget
			PauseMenuWidget = CreateWidget<UUserWidget>(GetWorld(), PauseMenuWidgetClass);

			if (PauseMenuWidget)
			{
				UnlockAchievement(FName("Pause"));

				UGameplayStatics::SetGamePaused(GetWorld(), true); //true = paused, false = resume.
				PauseMenuWidget->AddToViewport();

				// Game and UI so we can pause and unpause while the pause menu is up
				FInputModeGameAndUI InputMode;
				APlayerController* PC = Cast<APlayerController>(GetController());
				PC->SetInputMode(InputMode);

				AGameHUD* HUD = PC->GetHUD<AGameHUD>();
				HUD->HideHUD();
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("AGameCharacter Pause: PauseMenuWidget not valid !!! "));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, TEXT("AGameCharacter does not have PauseMenuWidgetClass set !!! "));
		}

	} // PAUSED
	else
	{
		Back();
	}
}

void AShooterCharacter::Back()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AGameCharacter::Back"));

	if (UGameplayStatics::IsGamePaused(GetWorld())) // PAUSED
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false); //true = paused, false = resume
		if (PauseMenuWidget)
		{
			PauseMenuWidget->RemoveFromParent();
		}
	}

	// Game Input Mode Only
	FInputModeGameOnly InputMode;
	APlayerController* PC = Cast<APlayerController>(GetController());
	PC->SetInputMode(InputMode);

	AGameHUD* HUD = PC->GetHUD<AGameHUD>();
	HUD->DisplayHUD();
}

void AShooterCharacter::UnlockAchievement(FName AchievementName)
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	UAchievementSubSystem* AchievementSybsystem = GameInstance->GetSubsystem<UAchievementSubSystem>();

	if (AchievementSybsystem->AchievementExists(AchievementName))
	{
		AchievementSybsystem->Unlock(AchievementName);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
			FString::Printf(TEXT("The following Achievement does not exist: %s"), *AchievementName.ToString()));
	}
}

void AShooterCharacter::AddAchievementProgress(FName AchievementName, float Progress)
{
	UGameInstance* gameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	UAchievementSubSystem* achievementSubsystem = gameInstance->GetSubsystem<UAchievementSubSystem>();

	if (achievementSubsystem->AchievementExists(AchievementName))
	{
		achievementSubsystem->AddAchievementProgress(AchievementName, Progress);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red,
			FString::Printf(TEXT("The following Achievement does not exist: %s"), *AchievementName.ToString()));
	}
}

