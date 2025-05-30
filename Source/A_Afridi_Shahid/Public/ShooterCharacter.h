// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BPI_PortalJumper.h"
#include "ColouredKey.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "ShooterCharacter.generated.h"

class ARocketLauncher;
class APortal2;
class APortalBulletBlue;
class APortalBulletRed;
class INiagaraChain;
class AGun;

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_Sprint UMETA(DisplayName = "Sprint"),
	ECS_GrabbingObject UMETA(DisplayName = "GrabbingObject"),
	ECS_SwitchWeapon UMETA(DisplayName = "SwitchWeapon"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_Stunned UMETA(DisplayName = "Stunned"),
	ECS_Death UMETA(DisplayName = "Death"),

	ECS_NAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_Unarmed UMETA(DisplayName = "Unarmed"),
	EWT_PortalGun UMETA(DisplayName = "PortalGun"),
	EWT_RocketLauncher UMETA(DisplayName = "RocketLauncher"),
};

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class A_AFRIDI_SHAHID_API AShooterCharacter : public ACharacter, public IBPI_PortalJumper
{
	GENERATED_BODY()

	/** Spring arm to control camera movement */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LeftFireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RightFireAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleDroneAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RotateGrabbedObjectXAxis;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RotateGrabbedObjectYAxis;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RotateGrabbedObjectZAxis;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* IncreaseGrabDistAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DecreaseGrabDistAction;

	

	/** Combat State, can only fire or reload if Unoccupied */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	EWeaponType CurrentWeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UPhysicsHandleComponent* PhysicsHandle;

	FVector PendingHitLocationOffset;
	FRotator PendingNormalRotator;
	bool bPendingIsOrange;
	bool IsGrabbingObject;


public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void StoreWeapon();
	UFUNCTION(BlueprintCallable)
	void RetrieveWeapon();

	void IncreaseGrabDistance();

	AShooterCharacter();

	// Override the interface function
	virtual void JumpPortal_Implementation(const FVector& TargetLocation, const FRotator& TargetRotation) override;

	/** Currently equipped Weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class AGun* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool SprintPressed;

	class UNiagaraComponent* NiagaraComp;

	class ACheckpoint* ActiveCheckpoint;

	virtual void UnPossessed() override;
	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintCallable)
	void SpawnedFirstPortalGun();

	UFUNCTION(BlueprintImplementableEvent)
	void SwitchTo(bool FPS, bool AlsoPlayMontage);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Drone, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Keys, meta = (AllowPrivateAccess = "true"))
	TArray<AColouredKey*> colouredKeys;

protected:
	virtual void BeginPlay();
	void DecreaseGrabDistance();
	AGun* SpawnPortalGunWeapon();
	AGun* SpawnRocketLauncherWeapon();
	void EquipWeapon(AGun* WeaponToEquip, FName SocketName);
	void WeaponSwitch();
	void ShowWeaponAfterSwitch();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PauseAction;

	// Back actions are used for unpausing the game
	// Some platforms use a different button for back, so we need two back actions
	// We will skip the one that is not correct

	/** Back Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* BackActionRight;

	/** Back Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* BackActionBottom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UForceFeedbackEffect* FireForceFeedbackEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UForceFeedbackEffect* PickupForceFeedbackEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UForceFeedbackEffect* HurtForceFeedbackEffect;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pause Menu", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> PauseMenuWidgetClass;

	UPROPERTY()
	class UUserWidget* PauseMenuWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pause Menu", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UUserWidget> ResultWidgetClass;

	UPROPERTY()
	class UUserWidget* ResultMenuWidget;

	void Die();

	class AGrabbingObject* GrabbingActor;
	class ATurret* GrabbingTurretActor;

	FVector GrabbedObjectOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float TotalEnemies;

	// for Widgets incrementing Score and progress.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float KilledEnemies_Score = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float AddSouls_Score = 0.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float AddExplosives_Score = 0.f;

	// Countdown Timer
	void CountDown();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	int32 Minutes = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	int32 Seconds = 0;

public:
	int32 GetMinutes() { return Minutes; }

private:
	UPrimitiveComponent* GrabbedComponent = nullptr;

	bool DoOnceGrabbedObjMessage = true;

protected:

	UFUNCTION(BlueprintCallable)
	void Pause();

public:
	UFUNCTION(BlueprintCallable)
	void Back();

	UFUNCTION()
    void PlayForceFeedbackEffect(UForceFeedbackEffect* FeedbackEffect);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float SelfDamageValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ImpactPaticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* DeathSound;

public:	

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void GetHit(int DamageValue);

	void IncrementKilledEnemiesScore();

	UFUNCTION(BlueprintImplementableEvent)
	void DisplayResult();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void CPP_ShowUIMessage(const FString& Message);

private:
	/** Achievement Stuff */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Achievement, meta = (AllowPrivateAccess = "true"))
	float StayStillAchievementTime = 5.0f;

	float StayStillTimeCounter = 0;
	bool StayStillAchievementUnlocked = false;

	public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Achievement, meta = (AllowPrivateAccess = "true"))
	bool bBothPortalsCreated = false;
	bool bOrangePortalPlaced = false;
	bool bBluePortalPlaced = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Achievement, meta = (AllowPrivateAccess = "true"))
	float checkpointsReached = 0;

	void UnlockAchievement(FName AchievementName);
	void AddAchievementProgress(FName AchievementName, float Progress);

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SwitchWeaponAction;

	void ActivateChainLightning(INiagaraChain* NiagaraChainInterface, AActor* Actor, const FVector& StartLocation, const FVector& EndLocation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AGun> PortalGunWeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ARocketLauncher> RocketLauncherWeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ARocketProjectile> RocketProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	float LineTraceDistance = 10000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	float GrabLineTraceDistance = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	class UNiagaraSystem* LightningEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<APortalBulletRed> BP_PortalBulletRed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<APortalBulletBlue> BP_PortalBulletBlue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<APortal2> PortalClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	USoundBase* PortalTeleportSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	USoundBase* DropSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ACheckpoint> CheckpointClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	float PortalProjectileSpawnedMoveSpeed = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	float GrabDistance = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	float GrabObjectRotateSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	bool ShowLineTrace;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	bool bPortalSurface;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	bool bPlayerJumped;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	float GrabDisIncSpeed = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	float ClampMinGrabDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	float ClampMaxGrabDistance = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	float shootingCooldownTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	float shootingCooldown = 1.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	bool CPP_FPS_ModeOn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	FVector StorePortalGunScale3D = FVector(0.5f, 0.5f, 0.5f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	FVector StoreRocketLauncherScale3D = FVector(0.5f, 0.5f, 0.5f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* EquipWeaponPickupEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
	USoundBase* WeaponSwitchSound;

	APortal2* PortalClassRef;

	AActor* ProjectileActor;

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
	void ShiftButtonPressed();
	void ShiftButtonReleased();
	void Jump();
	void StopJump();
	void PlayGunfireMontage();
	void LeftFirePressed();
	void FireRocket();
	void RightFirePressed();
	void SendBullet(bool bIsOrange);

	/** Called for rotating grabbed object input */
	void RotateGrabbedObjectOnXAxis();
	void RotateGrabbedObjectOnYAxis();
	void RotateGrabbedObjectOnZAxis();

	UFUNCTION()
	void OnMoveFinished();

	UFUNCTION(BlueprintImplementableEvent)
	void GetSurfaceType(FHitResult BeamHitResult);

	UFUNCTION(BlueprintImplementableEvent)
	void DisplayUICoolDown();

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult);
	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);
	AActor* GetBullet(bool bIsOrange);

protected:

	void ToggleDrone();
	// End of APawn interface

public:

	void SpawnRelocatePortalType(bool bIsOrange, FVector HitLocationOffset, FRotator NormalRotator);

	UFUNCTION(BlueprintCallable)
	void ReleaseOrDropObject();

	UFUNCTION(BlueprintCallable)
	void DeactivateNiagara();

	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return Camera; }

	// Jump Velocity variable
	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	float JumpVelocity;

	FORCEINLINE float GetJumpVelocity() const { return JumpVelocity; }
	FORCEINLINE void SetJumpVelocity(float NewJumpVelocity) { JumpVelocity = NewJumpVelocity; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	FORCEINLINE void SetCombatState(ECombatState NewState) { CombatState = NewState; }
};
