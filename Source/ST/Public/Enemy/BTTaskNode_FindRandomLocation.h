#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_FindRandomLocation.generated.h"

UCLASS()
class ST_API UBTTaskNode_FindRandomLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTaskNode_FindRandomLocation();

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	struct FBlackboardKeySelector LocationKey;

	UPROPERTY(EditAnywhere, Category = "Search", meta = (ClampMin = "100.0"))
	float SearchRadius = 1000.0f;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
