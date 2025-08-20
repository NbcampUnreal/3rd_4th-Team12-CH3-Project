#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "STEnemyStateComponent.h" 
#include "STMeleeAnimInstance.generated.h"

UCLASS()
class ST_API USTMeleeAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
    
	UFUNCTION()
	void OnStateChanged(EEnemyState NewState, EEnemyState PreviousState);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	bool bIsAttackingIn = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	bool bIsDead = false;

	FString GetEnemyStateString(EEnemyState State);
};
