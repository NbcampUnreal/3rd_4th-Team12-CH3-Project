#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "Enemy/STEnemyStateComponent.h"
#include "BTDecorator_CheckPatrolMode.generated.h"

UCLASS()
class ST_API UBTDecorator_CheckPatrolMode : public UBTDecorator
{
	GENERATED_BODY()
public:
	UBTDecorator_CheckPatrolMode();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	UPROPERTY(EditAnywhere, Category = "Patrol")
	EPatrolMode PatrolModeToCheck;
};
