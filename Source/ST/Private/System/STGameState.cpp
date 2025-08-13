#include "System/STGameState.h"

#include "System/STLog.h"

ASTGameState::ASTGameState()
{
	// FGameStateInfo struct 자체에서 초기화 리스트 할당함(초기화 불필요) 
}

const FGameStateInfo& ASTGameState::GetGameStateInfo() const
{
	return GameStateInfo;
}

void ASTGameState::SetStagePhase(EStagePhase NewStagePhase)
{
	GameStateInfo.StagePhase = NewStagePhase;
}

void ASTGameState::SetStageResult(EStageResult NewStageResult)
{
	GameStateInfo.StageResult = NewStageResult;
}

void ASTGameState::SetRemainingEnemies(int32 NewRemainingEnemies)
{
	GameStateInfo.RemainingEnemies = NewRemainingEnemies;
}

void ASTGameState::SetRemainingTime(int32 NewRemainingTime)
{
	// UE_LOG(LogSystem, Log, TEXT("ASTGameState::SetRemainingTime(%d) Start"), NewRemainingTime);

	GameStateInfo.RemainingTime = NewRemainingTime;		// 이제 이걸 저장할 필요가 있을까?
	OnRemainingTimeUpdated.Broadcast(NewRemainingTime);	// Delegate Broadcast
	
	// UE_LOG(LogSystem, Log, TEXT("ASTGameState::SetRemainingTime(%d) End"), NewRemainingTime);
}

void ASTGameState::SetBossPhase(int32 NewBossPhase)
{
	GameStateInfo.BossPhase = NewBossPhase;
}

void ASTGameState::SetStageGoalText(const FText& NewStageGoalText)
{
	GameStateInfo.StageGoalText = NewStageGoalText;
}

void ASTGameState::SetStageProgressList(const TArray<FText>& NewStageProgressList)
{
	GameStateInfo.StageProgressList = NewStageProgressList;
}
