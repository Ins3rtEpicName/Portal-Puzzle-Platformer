#include "RotatingLaserBase.h"
#include "Components/StaticMeshComponent.h"
#include "GrabbingObject.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
ARotatingLaserBase::ARotatingLaserBase()
{
	// Set this actor to call Tick() every frame
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh2);

	Top = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Top"));
	Top->SetupAttachment(ItemMesh2);

    bIsLaserActive = false;
    NiagaraComp = nullptr;

    LastHitGrabbingObject = nullptr;
}

// Called when the game starts or when spawned
void ARotatingLaserBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARotatingLaserBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    // Rotate the mesh
    FRotator RotationDelta = FRotator(0.f, CurrentRotationSpeed * DeltaTime, 0.f);
    ItemMesh2->AddLocalRotation(RotationDelta);

    if (bIsLaserActive)
    {
        PerformLineTraceAndNiagaraUpdate();
    }
}

void ARotatingLaserBase::ActivateLaserBeam()
{
    if (!bIsLaserActive)
    {
        bIsLaserActive = true;
        CurrentRotationSpeed = MaxRotationSpeed;
    }
}

void ARotatingLaserBase::DeactivateLaserBeam()
{
    if (bIsLaserActive)
    {
        bIsLaserActive = false;

        if (NiagaraComp)
        {
            NiagaraComp->DestroyComponent();
            NiagaraComp = nullptr;
            CurrentRotationSpeed = RotationSpeed;
        }

        if (LastHitGrabbingObject)
        {
            LastHitGrabbingObject->DeactivateLaserBeam();
            LastHitGrabbingObject = nullptr;
        }
    }
}

void ARotatingLaserBase::PerformLineTraceAndNiagaraUpdate()
{
    FVector StartLocation = Top->GetComponentLocation();
    FVector ForwardVector = Top->GetForwardVector();
    FVector EndLocation = StartLocation + (ForwardVector * LaserBeamEnd);

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);

    if (bHit)
    {
        EndLocation = HitResult.ImpactPoint;

        // Check if the hit actor is a GrabbingObject
        if (HitResult.GetActor() && HitResult.GetActor()->ActorHasTag(FName("grabbingObject")))
        {
            AGrabbingObject* GrabbingObj = Cast<AGrabbingObject>(HitResult.GetActor());
            if (GrabbingObj)
            {
                // If this is a new GrabbingObject, activate its laser
                if (LastHitGrabbingObject != GrabbingObj)
                {
                    if (LastHitGrabbingObject)
                    {
                        LastHitGrabbingObject->DeactivateLaserBeam();
                    }
                    GrabbingObj->ActivateLaserBeam();
                    LastHitGrabbingObject = GrabbingObj;
                }
            }
        }
        else
        {
            if (LastHitGrabbingObject)
            {
                LastHitGrabbingObject->DeactivateLaserBeam();
                LastHitGrabbingObject = nullptr;
            }
        }
    }
    else
    {
        // If the turret stops hitting a GrabbingObject, deactivate the last one
        if (LastHitGrabbingObject)
        {
            LastHitGrabbingObject->DeactivateLaserBeam();
            LastHitGrabbingObject = nullptr;
        }
    }

    // Spawn and update Niagara for the turret's laser
    if (!NiagaraComp && LightningEffect)
    {
        NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(),
            LightningEffect,
            StartLocation
        );
    }

    if (NiagaraComp)
    {
        NiagaraComp->SetNiagaraVariableVec3(TEXT("User.End"), EndLocation);
        NiagaraComp->SetWorldLocation(StartLocation);
    }
}
