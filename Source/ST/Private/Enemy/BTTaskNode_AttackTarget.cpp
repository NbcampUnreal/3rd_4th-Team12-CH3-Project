#include "Enemy/BTTaskNode_AttackTarget.h"
#include "Enemy/STEnemyBase.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTaskNode_AttackTarget::UBTTaskNode_AttackTarget()
{
	NodeName = TEXT("Attack Target");
}

EBTNodeResult::Type UBTTaskNode_AttackTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}
	auto* Enemy = Cast<ASTEnemyBase>(AIController->GetPawn());
	if (!IsValid(Enemy))
	{
		return EBTNodeResult::Failed;
	}
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return EBTNodeResult::Failed;
	}
	
	AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!IsValid(Target))
	{
		return EBTNodeResult::Failed;
	}

	Enemy->SetCurrentTarget(Target);
	Enemy->Attack();
	
	return EBTNodeResult::Succeeded;
}
