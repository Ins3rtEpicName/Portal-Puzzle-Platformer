// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portal2.generated.h"

UCLASS()
class A_AFRIDI_SHAHID_API APortal2 : public AActor
{
	GENERATED_BODY()
	
public:
    // Sets default values for this actor's properties
    APortal2();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Updates the capture component with portal simulation data
    void UpdateCapture(USceneCaptureComponent2D* CaptureComponent, USceneComponent* PSim);

    // Updates the portal camera position and orientation
    void UpdateCamera();
    void JumpPortal(UObject* Jumper, class UBoxComponent* ExitColliderPara, USceneComponent* TargetCapture, UObject* TriggerCollider);
    void ClearExitCollider(UObject* Jumper);

    UFUNCTION()
    void OnBeginOverlapCol1(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION()
    void OnEndOverlapCol1(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
    UFUNCTION()
    void OnBeginOverlapCol2(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    UFUNCTION()
    void OnEndOverlapCol2(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UBoxComponent* ExitCollider;

    UFUNCTION(BlueprintCallable, Category = "Portal")
    void PlacePortal(bool bIsOrange, FVector NewLocation, FRotator NewRotation);

private:

    // Root Component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal Components", meta = (AllowPrivateAccess = "true"))
    USceneComponent* DefaultSceneRoot;

    // D2 Hierarchy
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal Components", meta = (AllowPrivateAccess = "true"))
    USceneComponent* D2;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal Components", meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* Door2;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal Components", meta = (AllowPrivateAccess = "true"))
    USceneComponent* CamRoot2;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal Components", meta = (AllowPrivateAccess = "true"))
    USceneCaptureComponent2D* P2Capture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal Components", meta = (AllowPrivateAccess = "true"))
    USceneComponent* PSimD2;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal Components", meta = (AllowPrivateAccess = "true"))
    class UBoxComponent* Col2;

    // D1 Hierarchy
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal Components", meta = (AllowPrivateAccess = "true"))
    USceneComponent* D1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal Components", meta = (AllowPrivateAccess = "true"))
    UStaticMeshComponent* Door1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal Components", meta = (AllowPrivateAccess = "true"))
    USceneComponent* CamRoot1;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal Components", meta = (AllowPrivateAccess = "true"))
    USceneCaptureComponent2D* P1Capture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal Components", meta = (AllowPrivateAccess = "true"))
    USceneComponent* PSimD1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Portal Components", meta = (AllowPrivateAccess = "true"))
    class UBoxComponent* Col1;

    // Interpolate Portal spawn size
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
    FVector InterpPortalStartCurrentScale = FVector(0.1f, 0.1f, 0.1f); // Initial small scale

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
    FVector InterpPortalEndTargetScale = FVector(1.0f, 1.0f, 1.0f); // Target scale

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
    float InterpPortalSizeSpeed = 5.0f; // Speed of interpolation

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
    float FOVValueMin = 5.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings, meta = (AllowPrivateAccess = "true"))
    float FOVValueMAx = 180.f;

    bool bIsScalingD1 = false;
    bool bIsScalingD2 = false;
};

