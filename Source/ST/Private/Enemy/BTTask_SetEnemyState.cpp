#include "Enemy/BTTask_SetEnemyState.h"
#include "AIController.h"
#include "Enemy/STEnemyBase.h"

UBTTask_SetEnemyState::UBTTask_SetEnemyState()
{
	NodeName = TEXT("Set Enemy State");
}

EBTNodeResult::Type UBTTask_SetEnemyState::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	ASTEnemyBase* Enemy = Cast<ASTEnemyBase>(AIController->GetPawn());
	if (!Enemy || !Enemy->StateComponent)
	{
		return EBTNodeResult::Failed;
	}

	Enemy->StateComponent->SetState(StateToSet);

	return EBTNodeResult::Succeeded;
}
