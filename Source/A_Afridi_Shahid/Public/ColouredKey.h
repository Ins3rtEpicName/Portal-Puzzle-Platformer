// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "ColouredKey.generated.h"

UENUM(BlueprintType)        //"BlueprintType" is essential to include
enum class KeyColour : uint8
{
	RED			UMETA(DisplayName = "RED"),
	BLUE		UMETA(DisplayName = "BLUE"),
	GREEN		UMETA(DisplayName = "GREEN")
};

UCLASS()
class A_AFRIDI_SHAHID_API AColouredKey : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AColouredKey();

	/*enum KeyColour
	{
		RED,
		GREEN,
		BLUE,
	};*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Properties")
	KeyColour keyColourEnum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UBoxComponent* boxComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};
