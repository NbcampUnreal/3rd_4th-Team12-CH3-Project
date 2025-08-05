#include "System/STGameState.h"

ASTGameState::ASTGameState()
{
	StagePhase = EStagePhase::Start;
	RemainingEnemies = 0;
	RemainingTime = 0.0f;
	BossPhase = 1;
	StageResult = EStageResult::None;
}

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
