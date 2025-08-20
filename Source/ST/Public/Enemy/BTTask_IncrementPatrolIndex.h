﻿#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_IncrementPatrolIndex.generated.h"

UCLASS()
class ST_API UBTTask_IncrementPatrolIndex : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_IncrementPatrolIndex();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
