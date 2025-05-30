// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal2.h"
#include "Components/SceneComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "BPI_PortalJumper.h"
#include "ShooterCharacter.h"

APortal2::APortal2()
{
    // Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Root Component
    DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
    RootComponent = DefaultSceneRoot;

    // D1 Hierarchy
    D1 = CreateDefaultSubobject<USceneComponent>(TEXT("D1"));
    D1->SetupAttachment(DefaultSceneRoot);

    Door1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door1"));
    Door1->SetupAttachment(D1);

    PSimD1 = CreateDefaultSubobject<USceneComponent>(TEXT("PSimD1"));
    PSimD1->SetupAttachment(D1);
    PSimD1->SetWorldLocation(FVector(0.0f, 1000.f, 0.0));

    CamRoot1 = CreateDefaultSubobject<USceneComponent>(TEXT("CamRoot1"));
    CamRoot1->SetupAttachment(D1);

    P1Capture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("P1Capture"));
    P1Capture->SetupAttachment(CamRoot1);

    Col1 = CreateDefaultSubobject<UBoxComponent>(TEXT("Col1"));
    Col1->SetupAttachment(D1);
    Col1->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Col1->SetCollisionResponseToAllChannels(ECR_Ignore);
    Col1->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    // Bind the overlap events to custom functions
    Col1->OnComponentBeginOverlap.AddDynamic(this, &APortal2::OnBeginOverlapCol1);
    Col1->OnComponentEndOverlap.AddDynamic(this, &APortal2::OnEndOverlapCol1);

    // D2 Hierarchy
    D2 = CreateDefaultSubobject<USceneComponent>(TEXT("D2"));
    D2->SetupAttachment(DefaultSceneRoot);

    Door2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door2"));
    Door2->SetupAttachment(D2);

    PSimD2 = CreateDefaultSubobject<USceneComponent>(TEXT("PSimD2"));
    PSimD2->SetupAttachment(D2);

    CamRoot2 = CreateDefaultSubobject<USceneComponent>(TEXT("CamRoot2"));
    CamRoot2->SetupAttachment(D2);

    P2Capture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("P2Capture"));
    P2Capture->SetupAttachment(CamRoot2);

    Col2 = CreateDefaultSubobject<UBoxComponent>(TEXT("Col2"));
    Col2->SetupAttachment(D2);
    Col2->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    Col2->SetCollisionResponseToAllChannels(ECR_Ignore);
    Col2->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    // Bind the overlap events to custom functions
    Col2->OnComponentBeginOverlap.AddDynamic(this, &APortal2::OnBeginOverlapCol2);
    Col2->OnComponentEndOverlap.AddDynamic(this, &APortal2::OnEndOverlapCol2);
}

// Called when the game starts or when spawned
void APortal2::BeginPlay()
{
    Super::BeginPlay();

    UpdateCamera();
}

void APortal2::PlacePortal(bool bIsOrange, FVector NewLocation, FRotator NewRotation)
{
    AShooterCharacter* player = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (bIsOrange)
    {
        D1->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
        D1->SetWorldLocationAndRotation(NewLocation, NewRotation);

        bIsScalingD1 = true;

        if (player)
        {
			player->bBluePortalPlaced = true;
        }
    }
    else
    {
        D2->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
        D2->SetWorldLocationAndRotation(NewLocation, NewRotation);

        bIsScalingD2 = true;

        if (player)
        {
            player->bOrangePortalPlaced = true;
        }
    }

    if (player && player->bBluePortalPlaced && player->bOrangePortalPlaced)
    {
        player->bBothPortalsCreated = true;
        player->UnlockAchievement("Portal Apprentice");
    }
}

// Called every frame
void APortal2::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateCamera();

    if (bIsScalingD2)
    {
        // Interpolate CurrentScale towards TargetScale
        InterpPortalStartCurrentScale = FMath::VInterpTo(InterpPortalStartCurrentScale, InterpPortalEndTargetScale, DeltaTime, InterpPortalSizeSpeed);

        // Update the actor's scale
        D2->SetRelativeScale3D(InterpPortalStartCurrentScale);

        // Stop scaling if the portal has reached the target size
        if (InterpPortalStartCurrentScale.Equals(InterpPortalEndTargetScale, 0.01f)) // Tolerance of 0.01 to consider it "equal"
        {
            InterpPortalStartCurrentScale = FVector(0.1f, 0.1f, 0.1f);
            bIsScalingD2 = false; // Disable further scaling
        }
    }

    if (bIsScalingD1)
    {
        // Interpolate CurrentScale towards TargetScale
        InterpPortalStartCurrentScale = FMath::VInterpTo(InterpPortalStartCurrentScale, InterpPortalEndTargetScale, DeltaTime, InterpPortalSizeSpeed);

        // Update the actor's scale
        D1->SetRelativeScale3D(InterpPortalStartCurrentScale);

        // Stop scaling if the portal has reached the target size
        if (InterpPortalStartCurrentScale.Equals(InterpPortalEndTargetScale, 0.01f)) // Tolerance of 0.01 to consider it "equal"
        {
            InterpPortalStartCurrentScale = FVector(0.1f, 0.1f, 0.1f);
            bIsScalingD1 = false; // Disable further scaling
        }
    }
}

void APortal2::UpdateCapture(USceneCaptureComponent2D* CaptureComponent, USceneComponent* PSim)
{
    if (!CaptureComponent || !PSim)
    {
        UE_LOG(LogTemp, Warning, TEXT("CaptureComponent or PSim is null. Cannot execute UpdateCapture."));
        return;
    }

    CaptureComponent->SetRelativeLocation(PSim->GetRelativeLocation());

    // Get the world location of the CaptureComponent
    FVector StartLocation = CaptureComponent->GetComponentLocation();

    // Get the world location of the attached parent
    USceneComponent* AttachedParent = CaptureComponent->GetAttachParent();
    if (!AttachedParent) // Ensure AttachedParent is valid
    {
        UE_LOG(LogTemp, Warning, TEXT("CaptureComponent has no attached parent!"));
        return;
    }
    FVector TargetLocation = AttachedParent->GetComponentLocation();

    // Calculate the look-at rotation
    FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);

    // Set the world rotation of the CaptureComponent
    CaptureComponent->SetWorldRotation(LookAtRotation);

    FVector RelativeLocation = CaptureComponent->GetRelativeLocation();

    // Calculate the magnitude (vector length) of RelativeLocation
    float LocationMagnitude = UKismetMathLibrary::VSize(RelativeLocation);

    double MaxValue = UKismetMathLibrary::Max(LocationMagnitude, 1.0);
    double Result = UKismetMathLibrary::Divide_DoubleDouble(200, MaxValue);
    double Angle = UKismetMathLibrary::DegAtan(Result);

    float ClampedValue = UKismetMathLibrary::FClamp(Angle, FOVValueMin, FOVValueMAx);

    CaptureComponent->FOVAngle = ClampedValue;
}

void APortal2::UpdateCamera()
{
    // Get Player Camera Manager
    APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
    if (!CameraManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("CameraManager not found!"));
        return;
    }

    // Get the camera location
    FVector CameraLocation = CameraManager->GetCameraLocation();

    // Update PSimD1
    if (PSimD1)
    {
        PSimD1->SetWorldLocation(CameraLocation);
    }

    // Update PSimD2
    if (PSimD2)
    {
        PSimD2->SetWorldLocation(CameraLocation);
    }

    UpdateCapture(P1Capture, PSimD2);
    UpdateCapture(P2Capture, PSimD1);
}

void APortal2::JumpPortal(UObject* Jumper, UBoxComponent* ExitColliderPara, USceneComponent* TargetCapture, UObject* TriggerCollider)
{
    if (TriggerCollider != ExitCollider)
    {
        ExitCollider = ExitColliderPara;

        FRotator ComponentRotation = TargetCapture->GetComponentRotation();
        FVector ComponentLocation = ExitCollider->GetComponentLocation();

        // Check if Jumper implements the IBPI_PortalJumper interface
        if (Jumper->GetClass()->ImplementsInterface(UBPI_PortalJumper::StaticClass()))
        {
            FString ClassName = Jumper->GetClass()->GetName();

            IBPI_PortalJumper::Execute_JumpPortal(Jumper, ComponentLocation, ComponentRotation);
        }
    }
}

void APortal2::ClearExitCollider(UObject* Jumper)
{
    if (Jumper == ExitCollider)
    {
        ExitCollider = nullptr;
    }
}

void APortal2::OnBeginOverlapCol1(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    JumpPortal(OtherActor, Col2, P2Capture, OverlappedComponent);
}

void APortal2::OnEndOverlapCol1(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    ClearExitCollider(OverlappedComponent);
}

void APortal2::OnBeginOverlapCol2(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    JumpPortal(OtherActor, Col1, P1Capture, OverlappedComponent);
}

void APortal2::OnEndOverlapCol2(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    ClearExitCollider(OverlappedComponent);
}