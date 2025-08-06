#include "Enemy/STAIControllerBase.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Enemy/STEnemyBase.h"

ASTAIControllerBase::ASTAIControllerBase()
{
    PrimaryActorTick.bCanEverTick = true;

    AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
    SetPerceptionComponent(*AIPerception);

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 2000.f;
    SightConfig->LoseSightRadius = 2200.f;
    SightConfig->PeripheralVisionAngleDegrees = 80.f;
    SightConfig->SetMaxAge(6.0f);
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = 2000.f;
    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;

    AIPerception->ConfigureSense(*SightConfig);
    AIPerception->ConfigureSense(*HearingConfig);
    AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());

    BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));
}

void ASTAIControllerBase::StartBehaviorTree()
{
    if (BehaviorTreeAsset)
    {
    	RunBehaviorTree(BehaviorTreeAsset);
    } 
}

void ASTAIControllerBase::NotifyPlayerAttack(AActor* DamagedEnemy)
{
    AddToRecognitionGauge(60.0f);
}

void ASTAIControllerBase::AddToRecognitionGauge(float Delta)
{
    SetRecognitionGauge(RecognitionGauge + Delta);
}

void ASTAIControllerBase::SetRecognitionGauge(float NewValue)
{
    float Clamped = FMath::Clamp(NewValue, 0.f, RecognitionGaugeMax);
    if (!FMath::IsNearlyEqual(RecognitionGauge, Clamped))
    {
        RecognitionGauge = Clamped;
    	// Delegate로 UI에 알림
        OnRecognitionGaugeChanged.Broadcast(RecognitionGauge);
    }
}

void ASTAIControllerBase::BeginPlay()
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
    	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ASTAIControllerBase::OnPerceptionUpdated);
    }   
}

void ASTAIControllerBase::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (BehaviorTreeAsset && UseBlackboard(BehaviorTreeAsset->BlackboardAsset, BlackboardComp))
    {
    	RunBehaviorTree(BehaviorTreeAsset);
    }  
}

void ASTAIControllerBase::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!BlackboardComp)
    {
    	return;
    }
    if (!bPlayerInSight && RecognitionGauge <= 0.f)
    {
    	return;
    }

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    ASTEnemyBase* Enemy = Cast<ASTEnemyBase>(GetPawn());
    if (!PlayerPawn || !Enemy || !Enemy->StateComponent)
    {
    	return;
    }

    float Distance = FVector::Dist(Enemy->GetActorLocation(), PlayerPawn->GetActorLocation());
    float GainRate = GetRecognitionGaugeGainRate(Distance);

    if (bPlayerInSight)
    {
    	AddToRecognitionGauge(GainRate * DeltaSeconds);
    }
    else
    {
    	AddToRecognitionGauge(-35.f * DeltaSeconds);
    }
	
    if (RecognitionGauge >= 100.f)
    {
        BlackboardComp->SetValueAsObject(TEXT("TargetActor"), PlayerPawn);
        BlackboardComp->SetValueAsBool(TEXT("CanSeeTarget"), true);
        BlackboardComp->SetValueAsBool(TEXT("IsInvestigating"), false);
    }
    else if (RecognitionGauge >= 70.f)
    {
        BlackboardComp->SetValueAsBool(TEXT("CanSeeTarget"), false);
        BlackboardComp->SetValueAsVector(TEXT("TargetLastLocation"), PlayerPawn->GetActorLocation());
        BlackboardComp->SetValueAsBool(TEXT("IsInvestigating"), true);
    }

#if WITH_EDITOR
    FString GaugeInfo = FString::Printf(TEXT("Gauge: %.0f / %.0f"), RecognitionGauge, RecognitionGaugeMax);
    DrawDebugString(GetWorld(), Enemy->GetActorLocation() + FVector(0,0,120), GaugeInfo, nullptr, FColor::Orange, 0.f, true);
#endif
}

void ASTAIControllerBase::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (Actor != PlayerPawn)
    {
    	return;
    }

    if (Stimulus.Type == UAISense::GetSenseID(UAISense_Sight::StaticClass()))
    {
    	bPlayerInSight = Stimulus.WasSuccessfullySensed();
    }
    else if (Stimulus.Type == UAISense::GetSenseID(UAISense_Hearing::StaticClass()))
    {
        if (Stimulus.WasSuccessfullySensed())
            AddToRecognitionGauge(30.f);
    }
}

float ASTAIControllerBase::GetRecognitionGaugeGainRate(float Distance) const
{
	// 거리 기반 게이지 증가율 계산 (가까울수록 게이지가 빨리 참)
	// InputRange(최대 시야 거리 : 2000, 가까운 거리 : 300)
	// OutputRange(GainRate의 값이 Distance에 따라 20~110 으로 변화)
    return FMath::GetMappedRangeValueClamped(FVector2D(2000.f, 300.f), FVector2D(20.f, 120.f), Distance);
}
