#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_IsInvestigating.generated.h"

UCLASS()
class ST_API UBTTaskNode_IsInvestigating : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTaskNode_IsInvestigating();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;	
	
};
