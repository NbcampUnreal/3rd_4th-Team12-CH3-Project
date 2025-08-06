#include "Enemy/BTTask_SetPatrolWaypoint.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/STEnemyBase.h"
#include "NavigationSystem.h"
#include "NavMesh/RecastNavMesh.h" 


UBTTask_SetPatrolWaypoint::UBTTask_SetPatrolWaypoint()
{
	NodeName = TEXT("Set Patrol Waypoint as Move Target");
}

EBTNodeResult::Type UBTTask_SetPatrolWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	int32 Index = Enemy->CurrentPatrolIndex;
	Index = FMath::Clamp(Index, 0, Enemy->PatrolPoints.Num() - 1);
	AActor* PatrolPoint = Enemy->PatrolPoints[Index];

	FVector PatrolLoc = PatrolPoint->GetActorLocation();
	
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(Enemy->GetWorld());
	if (NavSys)
	{
		FNavLocation ProjectedLoc;
		// (검색반경 100 100 200으로 Nav 가능한 곳으로 대체 )
		if (NavSys->ProjectPointToNavigation(PatrolLoc, ProjectedLoc, FVector(100.f, 100.f, 200.f)))
		{
			PatrolLoc = ProjectedLoc.Location;
		}
	}

	auto BB = OwnerComp.GetBlackboardComponent();
	if (BB)
	{
		BB->SetValueAsVector(TEXT("PatrolLocation"), PatrolLoc);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
