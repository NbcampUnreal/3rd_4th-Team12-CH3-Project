#include "Enemy/BTDecorator_CheckPatrolMode.h"
#include "Enemy/STEnemyBase.h"
#include "AIController.h"

UBTDecorator_CheckPatrolMode::UBTDecorator_CheckPatrolMode()
{
	NodeName = TEXT("Check Patrol Mode");
}

bool UBTDecorator_CheckPatrolMode::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const auto AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return false;
	}

	const auto Enemy = Cast<ASTEnemyBase>(AIController->GetPawn());
	if (!Enemy || !Enemy->StateComponent)
	{
		return false;
	}

	return Enemy->StateComponent->IsInPatrolMode(PatrolModeToCheck);
}
