#include "Enemy/BTTaskNode_IsInvestigating.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTaskNode_IsInvestigating::UBTTaskNode_IsInvestigating()
{
	NodeName = TEXT("IsInvestigating False");
}

EBTNodeResult::Type UBTTaskNode_IsInvestigating::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsBool(TEXT("IsInvestigating"), false);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
