#include "System/STPlayerState.h"
#include "System/STGameInstance.h"
#include "System/STLog.h"

/********** public ***********/
ASTPlayerState::ASTPlayerState()
{
	
}

const FPlayerStateInfo& ASTPlayerState::GetPlayerStateInfo() const
{
	return PlayerStateInfo;
}

void ASTPlayerState::SetLastStage(EStageType NewStage)
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetSelectedCharacter(%s) Start"), *StaticEnum<ECharacterType>()->GetValueAsString(NewStage));
	PlayerStateInfo.LastStage = NewStage;
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetSelectedCharacter(%s) End"), *StaticEnum<ECharacterType>()->GetValueAsString(NewStage));
}

void ASTPlayerState::SetSelectedCharacter(ECharacterType NewCharacter)
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetSelectedCharacter(%s) Start"), *StaticEnum<ECharacterType>()->GetValueAsString(NewCharacter));
	
	PlayerStateInfo.SelectedCharacter = NewCharacter;
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetSelectedCharacter(%s) End"), *StaticEnum<ECharacterType>()->GetValueAsString(NewCharacter));
}

/********** protected *************/
void ASTPlayerState::BeginPlay()
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::BeginPlay() Start"));
	Super::BeginPlay();

	// 레벨 로드시 Game Instance에 저장된 정보 불러오기
	if (USTGameInstance* STGameInstance = Cast<USTGameInstance>(GetGameInstance()))
	{
		SetLastStage(STGameInstance->LastStage);
		SetSelectedCharacter(STGameInstance->SelectedCharacter);
	}
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::BeginPlay() End"));
}

/*
void ASTPlayerState::ResetPlayerState()
{
	SetScore(0.0f);	// PlayerState에서 정의된 private float Score;
	KillCount = 0;
	SelectedCharacter = ECharacterType::None;
}
*/
