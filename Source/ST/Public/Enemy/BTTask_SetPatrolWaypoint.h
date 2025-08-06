#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Enemy/STEnemyBase.h"
#include "BTTask_SetPatrolWaypoint.generated.h"

UCLASS()
class ST_API UBTTask_SetPatrolWaypoint : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_SetPatrolWaypoint();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
