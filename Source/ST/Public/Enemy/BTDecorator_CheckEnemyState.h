#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "Enemy/STEnemyStateComponent.h"
#include "BTDecorator_CheckEnemyState.generated.h"

UCLASS()
class ST_API UBTDecorator_CheckEnemyState : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_CheckEnemyState();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	UPROPERTY(EditAnywhere, Category = "State")
	EEnemyState StateToCheck;
};
