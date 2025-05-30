#include "Checkpoint.h"
#include "Components/BoxComponent.h"
#include "ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"

ACheckpoint::ACheckpoint()
{
    PrimaryActorTick.bCanEverTick = false;

    // Initialize BoxComponent
    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
    RootComponent = BoxComponent;

    // Set default collision settings
    BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BoxComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
    BoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    BoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

    // Bind the overlap event
    BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnOverlapBegin);

    RespawnLocation = GetActorLocation();
    bIsActive = true;
}

void ACheckpoint::BeginPlay()
{
    Super::BeginPlay();
}

void ACheckpoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    // Ensure OtherActor is valid and tagged as Player
    if (OtherActor && OtherActor->ActorHasTag(FName("Player")))
    {
        if (bIsActive)
        {
            AShooterCharacter* Player = Cast<AShooterCharacter>(OtherActor);
            if (Player)
            {
                ActivateCheckpoint(); // Activate this checkpoint
                Player->ActiveCheckpoint = this; // Set this checkpoint as the player's active checkpoint

                UE_LOG(LogTemp, Log, TEXT("Checkpoint activated at location: %s"), *RespawnLocation.ToString());
            }
        }
    }
}

void ACheckpoint::ActivateCheckpoint()
{
    // Play the activate sound
    if (LaserActivateSound)
    {
        UGameplayStatics::PlaySound2D(this, LaserActivateSound);
    }

    bIsActive = false;
    RespawnLocation = GetActorLocation(); // Update respawn location to current actor location

    AShooterCharacter* player = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
    if (player)
    {
        player->checkpointsReached += 1;
        if (player->checkpointsReached == 3)
        {
            // cleared level 1
            player->UnlockAchievement("Initiate Explorer");

            if (player->GetMinutes() > 1)
            {
	            // cleared level 1 within 25 mins
                player->UnlockAchievement("Speedrunner");
            }
        }

        /*if (player->checkpointsReached == 10)
        {
            player->DisplayResult();
        }*/
    }
}

FVector ACheckpoint::GetRespawnLocation() const
{
    return RespawnLocation;
}