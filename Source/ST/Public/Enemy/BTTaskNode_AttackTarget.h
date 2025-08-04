#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_AttackTarget.generated.h"

UCLASS()
class ST_API UBTTaskNode_AttackTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTaskNode_AttackTarget();

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector TargetActorKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
