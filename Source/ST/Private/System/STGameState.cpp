#include "System/STGameState.h"

ASTGameState::ASTGameState()
{
	// FGameStateInfo struct 자체에서 기본값으로 초기화됨 
}

const FGameStateInfo& ASTGameState::GetGameStateInfo() const
{
	return GameStateInfo;
}

void ASTGameState::SetStagePhase(EStagePhase NewStagePhase)
{
	GameStateInfo.StagePhase = NewStagePhase;
}

/* setter(GameMode 전용) */
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
	GameStateInfo.RemainingTime = NewRemainingTime;		// 이제 이걸 저장할 필요가 있을까?
	OnRemainingTimeUpdated.Broadcast(NewRemainingTime);	// Delegate Broadcast
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
