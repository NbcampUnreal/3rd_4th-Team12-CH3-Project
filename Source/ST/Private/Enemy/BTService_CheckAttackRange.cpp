#include "Enemy/BTService_CheckAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/STEnemyBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "GameFramework/Actor.h"

UBTService_CheckAttackRange::UBTService_CheckAttackRange()
{
	NodeName = TEXT("Check Attack Range");
	bNotifyBecomeRelevant = true;
}

void UBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return;
	}
	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	APawn* SelfPawn = OwnerComp.GetAIOwner()->GetPawn();

	bool bInRange = false;
	bool bInApproachDistance = false;
	if (TargetActor && SelfPawn)
	{
		ASTEnemyBase* EnemyBase = Cast<ASTEnemyBase>(SelfPawn);
		if (EnemyBase)
		{
			EnemyAttackRange  = EnemyBase->AttackRange;
			EnemyApproachDistance  = EnemyBase->ApproachDistance;
		}
		
		float Dist = FVector::Dist(TargetActor->GetActorLocation(), SelfPawn->GetActorLocation());
		bInRange = (Dist <= EnemyAttackRange);
		bInApproachDistance = (Dist <= EnemyApproachDistance );
	}
	
	BB->SetValueAsBool(TEXT("InAttackRange"), bInRange);
	BB->SetValueAsBool(TEXT("InApproachDistance"), bInApproachDistance);
}
