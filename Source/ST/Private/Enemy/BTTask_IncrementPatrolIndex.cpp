#include "Enemy/BTTask_IncrementPatrolIndex.h"
#include "AIController.h"
#include "Enemy/STEnemyBase.h"

UBTTask_IncrementPatrolIndex::UBTTask_IncrementPatrolIndex()
{
	NodeName = TEXT("Increment Patrol Index");
}

EBTNodeResult::Type UBTTask_IncrementPatrolIndex::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}
	auto Enemy = Cast<ASTEnemyBase>(AIController->GetPawn());
	if (!Enemy || Enemy->PatrolPoints.Num() == 0)
	{
		return EBTNodeResult::Failed;
	}

	Enemy->CurrentPatrolIndex = (Enemy->CurrentPatrolIndex + 1) % Enemy->PatrolPoints.Num();
	return EBTNodeResult::Succeeded;
}
