// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class A_AFRIDI_SHAHID_API UEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaSeconds);

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed;

	class AEnemy* Enemy;
};
