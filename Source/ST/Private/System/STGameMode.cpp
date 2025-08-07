#include "System/STGameMode.h"

#include "SubobjectDataSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "System/STGameState.h"
#include "Enemy/STEnemyBase.h"
#include "System/StageTimeLimitData.h"
#include "System/STGameInstance.h"
#include "System/STLog.h"

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
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::BeginPlay() Start"));
	Super::BeginPlay();

	OnPlayerEnteredClearZone.AddDynamic(this, &ASTGameMode::HandlePlayerEnteredClearZone);	// Delegate에 함수 바인딩

	FString CurrentStageName = UGameplayStatics::GetCurrentLevelName(GetWorld());	// TimeLimit을 csv 파일로 관리
	StageTimeLimit = GetStageTimeLimit(CurrentStageName);
	RemainingTime = StageTimeLimit;

	SetStagePhase(EStagePhase::Start);
	
	ResetStage();
	StartStage();
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::BeginPlay() End"));
}

void ASTGameMode::StartStage()
{
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::StartStage() Start"));
	// 1회성 타이머(루프X)로 제한시간 후 자동 GameOver
	GetWorldTimerManager().SetTimer(StageTimerHandle, this, &ASTGameMode::OnTimeOver, StageTimeLimit, false);

	SetStagePhase(EStagePhase::InProgress);
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::StartStage() End"));
}

void ASTGameMode::OnEnemyKilled()
{
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::OnEnemyKilled() Start"));
	DeadEnemies++;
	if (ASTGameState* STGameState = GetGameState<ASTGameState>())
		STGameState->SetRemainingEnemies(TotalEnemies - DeadEnemies);
	
	// TODO: 모든 적을 죽인 경우에 UI로 특정 위치로 이동하세요 라는 메시지 주기 (특정위치에 파티클 등으로 강조하고)
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::OnEnemyKilled() End"));
}

void ASTGameMode::OnPlayerDied()
{
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::OnPlayerDied() Start"));
	SetStagePhase(EStagePhase::Fail);
	EndStage(EStageResult::Fail);
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::OnPlayerDied() End"));
}

void ASTGameMode::SetBossPhase(int32 NewPhase)
{
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::SetBossPhase() Start"));
	BossPhase = NewPhase;
	if (ASTGameState* STGameState = GetGameState<ASTGameState>())
		STGameState->SetBossPhase(BossPhase);
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::SetBossPhase() End"));
}

void ASTGameMode::OnTimeOver()
{
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::OnTimeOver() Start"));
	SetStagePhase(EStagePhase::Fail);
	EndStage(EStageResult::Fail);
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::OnTimeOver() End"));
}

float ASTGameMode::GetRemainingTime() const
{
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::GetRemainingTime() Start"));
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::GetRemainingTime() End"));
	return GetWorldTimerManager().GetTimerRemaining(StageTimerHandle);
}


void ASTGameMode::EndStage(EStageResult Result)
{
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::EndStage() Start"));
	GetWorldTimerManager().ClearTimer(StageTimerHandle);
	// 여기서 레벨 전환 등 처리는 UI/블루프린트에서
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::EndStage() End"));
}

// NOTE: HandlePlayerEnteredClearZone delegate 연결로 사용 안하는 중
void ASTGameMode::CheckStageClear()
{
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::CheckStageClear() Start"));
	
	if (DeadEnemies >= TotalEnemies)
	{
		SetStagePhase(EStagePhase::Completed);
		EndStage(EStageResult::Clear);
	}
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::CheckStageClear() End"));
}

void ASTGameMode::ResetStage()
{
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::ResetStage() Start"));
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
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::ResetStage() End"));
}

float ASTGameMode::GetStageTimeLimit(const FString StageName) const
{
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::GetStageTimeLimit() Start"));
	if (!StageTimeLimit) return 10.0f; // 기본값 (300.0f), 테스트 (30.0f)

	FStageTimeLimitRow* Row = StageTimeTable->FindRow<FStageTimeLimitRow>(*StageName, TEXT("LookupStageTimeLimit"));
	if (Row)
	{
		UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::GetStageTimeLimit() End"));
		return Row->TimeLimit;
	}
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::GetStageTimeLimit() End"));
	return 10.0f;
}

void ASTGameMode::HandlePlayerEnteredClearZone()
{
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::HandlePlayerEnteredClearZone() Start"));
	if (DeadEnemies >= TotalEnemies)
	{
		UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::HandlePlayerEnteredClearZone() Stage Clear"));
		SetStagePhase(EStagePhase::Completed);
		EndStage(EStageResult::Clear);

		// 현재 스테이지 정보 가져오기
		USTGameInstance* STGameInstance = GetGameInstance<USTGameInstance>();
		if (!STGameInstance) return;

		EStageType NextStage = EStageType::None;
		UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::HandlePlayerEnteredClearZone() LastStage : %s"), *StaticEnum<EStageType>()->GetValueAsString(STGameInstance->LastStage));
		switch (STGameInstance->LastStage)
		{
			case EStageType::Stage1:
				NextStage = EStageType::Stage2;
				break;
			case EStageType::Stage2:
				NextStage = EStageType::Stage3;
				break;
			case EStageType::Stage3:
				// TODO: 엔딩으로 이동하거나 따로 처리하기
				// NextStage = EStageType::Ending;
				break;
			default:
				break;
		}

		if (NextStage != EStageType::None)
		{
			STGameInstance->LastStage = NextStage;
			STGameInstance->GoToLevel(NextStage);
		}
		else
		{
			// TODO: 엔딩처리 또는 결과화면이 뜨게 하기
			// STGameInstance->GoToLevel(EStage::Ending); 
		}
	}
	
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::HandlePlayerEnteredClearZone() End"));
}

void ASTGameMode::SetStagePhase(EStagePhase NewPhase)
{
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::SetStagePhase() Start"));
	if (ASTGameState* STGameState = GetGameState<ASTGameState>())
		STGameState->SetStagePhase(NewPhase);
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::SetStagePhase() End"));
}
