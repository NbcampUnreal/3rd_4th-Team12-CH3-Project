#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "STRangedAnimInstance.generated.h"

UCLASS()
class ST_API USTRangedAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	bool bIsAttackingIn = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	bool bIsReloading = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	bool bIsDead = false;
};
