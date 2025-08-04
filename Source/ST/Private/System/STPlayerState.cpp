#include "System/STPlayerState.h"

ASTPlayerState::ASTPlayerState()
{
	ResetPlayerState();
}

void ASTPlayerState::ResetPlayerState()
{
	SetScore(0.0f);	// PlayerState에서 정의된 private float Score;
	KillCount = 0;
	SelectedCharacter = ECharacterType::None;
}
