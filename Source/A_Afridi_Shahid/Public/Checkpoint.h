#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Checkpoint.generated.h"

class UBoxComponent;

UCLASS()
class A_AFRIDI_SHAHID_API ACheckpoint : public AActor
{
    GENERATED_BODY()

public:
    ACheckpoint();

    void ActivateCheckpoint();

    FVector GetRespawnLocation() const;

    // Respawn location for the player
    UPROPERTY(EditAnywhere, Category = "Checkpoint")
    FVector RespawnLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Settings)
    USoundBase* LaserActivateSound;

protected:
    virtual void BeginPlay() override;


    // Flag to indicate if this checkpoint is active
    bool bIsActive;

    // Box component for collision
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBoxComponent* BoxComponent;

    // Overlap function
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

};
