#include "Enemy/BTDecorator_CheckEnemyState.h"
#include "Enemy/STEnemyBase.h"
#include "AIController.h"

UBTDecorator_CheckEnemyState::UBTDecorator_CheckEnemyState()
{
	NodeName = TEXT("Check Enemy State");
}

bool UBTDecorator_CheckEnemyState::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
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

	return Enemy->StateComponent->IsInState(StateToCheck);
}
