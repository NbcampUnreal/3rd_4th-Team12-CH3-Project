#include "System/STGameState.h"

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

void ASTGameState::SetRemainingTime(float NewRemainingTime)
{
	GameStateInfo.RemainingTime = NewRemainingTime;
}

void ASTGameState::SetBossPhase(int32 NewBossPhase)
{
	GameStateInfo.BossPhase = NewBossPhase;
}


/*
EStagePhase ASTGameState::GetStagePhase() const
{
	return StagePhase;
}

int32 ASTGameState::GetRemainingEnemies() const
{
	return RemainingEnemies;
}

float ASTGameState::GetRemainingTime() const
{
	return RemainingTime;
}

int32 ASTGameState::GetBossPhase() const
{
	return BossPhase;
}

EStageResult ASTGameState::GetStageResult() const
{
	return StageResult;
}

void ASTGameState::SetStagePhase(EStagePhase NewPhase)
{
	StagePhase = NewPhase;
}

void ASTGameState::SetRemainingEnemies(int32 NewCount)
{
	RemainingEnemies = NewCount;
}

void ASTGameState::SetRemainingTime(float NewTime)
{
	RemainingTime = NewTime;
}

void ASTGameState::SetBossPhase(int32 NewPhase)
{
	BossPhase = NewPhase;
}

void ASTGameState::SetStageResult(EStageResult NewResult)
{
	StageResult = NewResult;
}
*/
