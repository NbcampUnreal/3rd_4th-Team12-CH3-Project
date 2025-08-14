#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_HasValidSkill.generated.h"

UCLASS()
class ST_API UBTDecorator_HasValidSkill : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_HasValidSkill();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector HasValidSkillKey;
};
