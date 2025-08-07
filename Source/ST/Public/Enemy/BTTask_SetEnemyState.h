#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Enemy/STEnemyStateComponent.h"
#include "BTTask_SetEnemyState.generated.h"

UCLASS()
class ST_API UBTTask_SetEnemyState : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SetEnemyState();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "State")
	EEnemyState StateToSet;
};