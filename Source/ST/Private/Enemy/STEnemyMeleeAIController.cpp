#include "Enemy/STEnemyMeleeAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Enemy/STEnemyStateComponent.h"
#include "Enemy/STEnemyBase.h"

ASTEnemyMeleeAIController::ASTEnemyMeleeAIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SetPerceptionComponent(*AIPerception);

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 1200.f;
	SightConfig->LoseSightRadius = 1400.f;
	SightConfig->PeripheralVisionAngleDegrees = 90.f;
	SightConfig->SetMaxAge(6.0f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());

	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));

}

void ASTEnemyMeleeAIController::StartBehaviorTree()
{
	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

void ASTEnemyMeleeAIController::BeginPlay()
{
	Super::BeginPlay();

	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsBool(TEXT("CanSeeTarget"), false);
		BlackboardComp->SetValueAsBool(TEXT("IsInvestigating"), false);
		StartBehaviorTree();
	}

	if (AIPerception)
	{
		AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ASTEnemyMeleeAIController::OnPerceptionUpdated);
	}
}

void ASTEnemyMeleeAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTreeAsset)
	{
		if (UseBlackboard(BehaviorTreeAsset->BlackboardAsset, BlackboardComp))
		{
			RunBehaviorTree(BehaviorTreeAsset);
		}
	}
}

void ASTEnemyMeleeAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Actor || Actor != PlayerPawn || !BlackboardComp)
	{
		return;
	}

	ASTEnemyBase* Enemy = Cast<ASTEnemyBase>(GetPawn());
	if (!Enemy || !Enemy->StateComponent)
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		BlackboardComp->SetValueAsObject(TEXT("TargetActor"), Actor);
		BlackboardComp->SetValueAsBool(TEXT("CanSeeTarget"), true);
		BlackboardComp->SetValueAsBool(TEXT("IsInvestigating"), false);
		Enemy->StateComponent->SetState(EEnemyState::Chase);
	}
	else
	{
		BlackboardComp->SetValueAsBool(TEXT("CanSeeTarget"), false);
		BlackboardComp->SetValueAsVector(TEXT("TargetLastLocation"), Actor->GetActorLocation());
		BlackboardComp->SetValueAsBool(TEXT("IsInvestigating"), true);
		Enemy->StateComponent->SetState(EEnemyState::Investigation);
	}
}
