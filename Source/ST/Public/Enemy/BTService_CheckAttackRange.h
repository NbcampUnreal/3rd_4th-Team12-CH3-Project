#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CheckAttackRange.generated.h"

UCLASS()
class ST_API UBTService_CheckAttackRange : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_CheckAttackRange();

protected:
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector TargetActorKey;
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector InAttackRangeKey;
	float AttackRange = 200.f;
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
