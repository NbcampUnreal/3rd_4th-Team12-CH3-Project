#include "System/STGameState.h"

ASTGameState::ASTGameState()
{
	StagePhase = EStagePhase::Start;
	RemainingEnemies = 0;
	RemainingTime = 0.0f;
	BossPhase = 1;
	StageResult = EStageResult::None;
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
