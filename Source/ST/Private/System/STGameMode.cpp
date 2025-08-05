#include "System/STGameMode.h"

#include "SubobjectDataSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "System/STGameState.h"
#include "Enemy/STEnemyBase.h"
#include "System/StageTimeLimitData.h"

ASTGameMode::ASTGameMode()
{
	TotalEnemies = 0;
	DeadEnemies = 0;
	StageTimeLimit = 300.0f;
	RemainingTime = StageTimeLimit;
	BossPhase = 1;
}

void ASTGameMode::BeginPlay()
{
	Super::BeginPlay();

	FString CurrentStageName = UGameplayStatics::GetCurrentLevelName(GetWorld());	// TimeLimit을 csv 파일로 관리
	StageTimeLimit = GetStageTimeLimit(CurrentStageName);
	RemainingTime = StageTimeLimit;

	SetStagePhase(EStagePhase::Start);
	
	ResetStage();
	StartStage();
}

void ASTGameMode::StartStage()
{
	// 1회성 타이머(루프X)로 제한시간 후 자동 GameOver
	GetWorldTimerManager().SetTimer(StageTimerHandle, this, &ASTGameMode::OnTimeOver, StageTimeLimit, false);

	SetStagePhase(EStagePhase::InProgress);
}

void ASTGameMode::OnEnemyKilled()
{
	DeadEnemies++;
	if (ASTGameState* STGameState = GetGameState<ASTGameState>())
		STGameState->SetRemainingEnemies(TotalEnemies - DeadEnemies);

	// CheckStageClear();	// TODO: 매번 체크하지말고, 특정 Spot에 이동하면 체크하는 방식으로
	// TODO: 모든 적을 죽인 경우에 UI로 특정 위치로 이동하세요 라는 메시지 주기 (특정위치에 파티클 등으로 강조하고)
}

void ASTGameMode::OnPlayerDied()
{
	SetStagePhase(EStagePhase::Fail);
	EndStage(EStageResult::Fail);
}

void ASTGameMode::SetBossPhase(int32 NewPhase)
{
	BossPhase = NewPhase;
	if (ASTGameState* STGameState = GetGameState<ASTGameState>())
		STGameState->SetBossPhase(BossPhase);
}

void ASTGameMode::OnTimeOver()
{
	SetStagePhase(EStagePhase::Fail);
	EndStage(EStageResult::Fail);
}

float ASTGameMode::GetRemainingTime() const
{
	return GetWorldTimerManager().GetTimerRemaining(StageTimerHandle);
}


void ASTGameMode::EndStage(EStageResult Result)
{
	GetWorldTimerManager().ClearTimer(StageTimerHandle);
	// 여기서 레벨 전환 등 처리는 UI/블루프린트에서
}

void ASTGameMode::CheckStageClear()
{
	if (DeadEnemies >= TotalEnemies)
	{
		SetStagePhase(EStagePhase::Completed);
		EndStage(EStageResult::Clear);
	}
}

void ASTGameMode::ResetStage()
{
	DeadEnemies = 0;
	BossPhase = 1;

	TArray<AActor*> EnemyActors;	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASTEnemyBase::StaticClass(),EnemyActors);
	TotalEnemies = EnemyActors.Num();
	RemainingTime = StageTimeLimit;

	/* 상태값 state에 전달 */
	if (ASTGameState* STGameState = GetGameState<ASTGameState>())
	{
		STGameState->SetStagePhase(EStagePhase::Start);
		STGameState->SetBossPhase(BossPhase);
		STGameState->SetRemainingEnemies(TotalEnemies - DeadEnemies);
		STGameState->SetRemainingTime(RemainingTime);
		STGameState->SetStageResult(EStageResult::None);
	}
}

float ASTGameMode::GetStageTimeLimit(const FString StageName) const
{
	if (!StageTimeLimit) return 10.0f; // 기본값 (300.0f), 테스트 (30.0f)

	FStageTimeLimitRow* Row = StageTimeTable->FindRow<FStageTimeLimitRow>(*StageName, TEXT("LookupStageTimeLimit"));
	if (Row)
	{
		return Row->TimeLimit;
	}
	return 10.0f;
}

void ASTGameMode::SetStagePhase(EStagePhase NewPhase)
{
	if (ASTGameState* STGameState = GetGameState<ASTGameState>())
		STGameState->SetStagePhase(NewPhase);
}
