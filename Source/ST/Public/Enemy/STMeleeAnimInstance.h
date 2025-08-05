#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "STMeleeAnimInstance.generated.h"

UCLASS()
class ST_API USTMeleeAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	bool bIsAttackingIn = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	bool bIsDead = false;
};
