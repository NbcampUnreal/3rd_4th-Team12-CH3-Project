#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "STEnemySkillTypes.h"
#include "BTService_UpdateDistanceAndCooldown.generated.h"

UCLASS()
class ST_API UBTService_UpdateDistanceAndCooldown : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_UpdateDistanceAndCooldown();

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	void UpdateSkillSelection(UBehaviorTreeComponent& OwnerComp);

public:
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector DistanceToTargetKey;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector SkillToUseKey;

private:
	EEnemySkillID SelectBestSkillByCooldown(class ASTEnemyBoss* Boss, float Distance);
};
