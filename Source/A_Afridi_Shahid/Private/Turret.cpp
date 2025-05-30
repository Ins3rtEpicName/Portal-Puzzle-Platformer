#include "Turret.h"
#include "GrabbingObject.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "ShooterCharacter.h"

ATurret::ATurret()
{
    PrimaryActorTick.bCanEverTick = true;

    ItemMesh2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    SetRootComponent(ItemMesh2);

    Top = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Top"));
    Top->SetupAttachment(ItemMesh2);
}

void ATurret::ShowHideUI(bool Show)
{
    ShowHideUIinBP(Show);
}

void ATurret::BeginPlay()
{
    Super::BeginPlay();
    Tags.Add(FName("GrabbingObject"));
}

void ATurret::PerformNiagaraChainSphereTrace_Implementation()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, GetName());
}

void ATurret::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    /*FVector StartLocation = Top->GetComponentLocation();
    FVector ForwardVector = Top->GetForwardVector();*/

    FVector StartLocation = ItemMesh2->GetRelativeLocation();
    FVector ForwardVector = ItemMesh2->GetForwardVector();

    FVector EndLocation = StartLocation + (GetActorRightVector() * LaserBeamEnd);

    FHitResult HitResult;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);

    if (bHit)
    {
        EndLocation = HitResult.ImpactPoint;
        AActor* HitActor = HitResult.GetActor();
        if (HitActor)
        {
            if (HitActor->ActorHasTag(FName("grabbingObject")))
            {
                AGrabbingObject* GrabbingObj = Cast<AGrabbingObject>(HitActor);
                if (GrabbingObj && LastHitGrabbingObject != GrabbingObj)
                {
                    if (LastHitGrabbingObject)
                        LastHitGrabbingObject->DeactivateLaserBeam();
                    GrabbingObj->ActivateLaserBeam();
                    LastHitGrabbingObject = GrabbingObj;
                }
            }
            else if (HitActor->ActorHasTag(FName("Player")))
            {
                if (AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(HitActor))
                {
                    ShooterCharacter->GetHit(Damage);
                    ShooterCharacter->UnlockAchievement("Laser Toast");
                }
            }
        }
    }
    else if (LastHitGrabbingObject)
    {
        LastHitGrabbingObject->DeactivateLaserBeam();
        LastHitGrabbingObject = nullptr;
    }

    if (!NiagaraComp && LightningEffect)
    {
        NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(), LightningEffect, StartLocation);
    }
    if (NiagaraComp)
    {
        NiagaraComp->SetNiagaraVariableVec3(TEXT("User.End"), EndLocation);
        NiagaraComp->SetWorldLocation(StartLocation);
    }
}
