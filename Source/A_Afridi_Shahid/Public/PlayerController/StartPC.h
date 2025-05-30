#pragma once

#include "CoreMinimal.h"
#include "PlayerController/BasePC.h"
#include "StartPC.generated.h"

UCLASS()
class A_AFRIDI_SHAHID_API AStartPC : public ABasePC
{

	GENERATED_BODY()

public:
	virtual bool InputKey(const FInputKeyParams& Params) override;

	UPROPERTY(EditDefaultsOnly)
	const TSoftObjectPtr<UWorld> NextLevel;
};
