// Fill out your copyright notice in the Description page of Project Settings.


#include "GrabbingObject.h"

#include "Door.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGrabbingObject::AGrabbingObject()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    SetRootComponent(ItemMesh);

    Top = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Top"));
    Top->SetupAttachment(ItemMesh);

    bIsLaserActive = false;
    NiagaraComp = nullptr;
}

void AGrabbingObject::PerformNiagaraChainSphereTrace_Implementation()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, GetName());
}

// Called when the game starts or when spawned
void AGrabbingObject::BeginPlay()
{
    Super::BeginPlay();

    Tags.Add(FName("GrabbingObject"));
}

void AGrabbingObject::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (ParticipateInLaser == false) return;

    if (bIsLaserActive)
    {
        PerformLineTraceAndNiagaraUpdate();
    }
}

void AGrabbingObject::JumpPortal_Implementation(const FVector& TargetLocation, const FRotator& TargetRotation)
{
    AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
    if (ShooterCharacter)
    {
        ShooterCharacter->ReleaseOrDropObject();
    }

    SetActorLocationAndRotation(TargetLocation, TargetRotation);
}

void AGrabbingObject::ShowHideUI(bool Show)
{
	ShowHideUIinBP(Show);
}

void AGrabbingObject::ActivateLaserBeam()
{
    if (ParticipateInLaser == false) return;

    if (!bIsLaserActive)
    {
        bIsLaserActive = true;

        // Play the activate sound
        if (LaserActivateSound)
        {
            UGameplayStatics::PlaySound2D(this, LaserActivateSound);
        }
    }
}

void AGrabbingObject::DeactivateLaserBeam()
{
    if (ParticipateInLaser == false) return;

    if (bIsLaserActive)
    {
        bIsLaserActive = false;

        // Play the deactivate sound
        if (LaserDeactivateSound)
        {
            UGameplayStatics::PlaySound2D(this, LaserDeactivateSound);
        }


        if (NiagaraComp)
        {
            NiagaraComp->DestroyComponent();
            NiagaraComp = nullptr;
        }

        if (LastHitGrabbingObject)
        {
            LastHitGrabbingObject->DeactivateLaserBeam();
            LastHitGrabbingObject = nullptr;
        }
    }
}

void AGrabbingObject::PerformLineTraceAndNiagaraUpdate()
{
    if (ParticipateInLaser == false) return;

    FVector StartLocation = Top->GetComponentLocation();
    FVector ForwardVector = Top->GetUpVector();
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
        else if (HitResult.GetActor() && (HitResult.GetActor()->ActorHasTag(FName("Door"))))
        {
            if (bPerformLineTrace == false)
            {
                ADoor* Door = Cast<ADoor>(HitResult.GetActor());
                if (Door)
                {
                    Door->OpenDoors();
                    bPerformLineTrace = true;
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
