#include "Enemy/BTDecorator_IsNotAttacking.h"
#include "Enemy/STEnemyBase.h"
#include "AIController.h"

UBTDecorator_IsNotAttacking::UBTDecorator_IsNotAttacking()
{
	NodeName = TEXT("Is Not Attacking");
}

bool UBTDecorator_IsNotAttacking::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return false;
	}
	ASTEnemyBase* OwnerPawn = Cast<ASTEnemyBase>(AIController->GetPawn());
	
	return OwnerPawn && OwnerPawn->IsAttacking();
}
