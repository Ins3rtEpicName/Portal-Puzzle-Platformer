#pragma once

#include "CoreMinimal.h"
#include "PlayerController/BasePC.h"
#include "GamePC.generated.h"

class UInputMappingContext;

UCLASS()
class A_AFRIDI_SHAHID_API AGamePC : public ABasePC
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputMapping")
	UInputMappingContext* InputMappingContext;

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;
};
