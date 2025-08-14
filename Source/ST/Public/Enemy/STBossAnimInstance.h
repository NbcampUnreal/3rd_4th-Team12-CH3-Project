#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enemy/STEnemyStateComponent.h"
#include "STBossAnimInstance.generated.h"

UCLASS()
class ST_API USTBossAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;

	UFUNCTION()
	void OnStateChanged(EEnemyState NewState, EEnemyState PreviousState);

	// 공통 상태 분기 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	bool bIsIdle = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	bool bIsPatrol = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	bool bIsChase = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	bool bIsAttack = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	bool bIsInvestigating = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	bool bIsDead = false;

	// 보스 페이즈
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	bool bIsPhaseChangingStart = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	bool bIsPhaseChangingEnd = false;
	
	UFUNCTION(BlueprintPure)
	FString GetEnemyStateString(EEnemyState State);
};
