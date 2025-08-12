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

void ASTPlayerState::SetSelectedCharacter(ECharacterType NewCharacter)
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetSelectedCharacter(%s) Start"), *StaticEnum<ECharacterType>()->GetValueAsString(NewCharacter));
	
	PlayerStateInfo.SelectedCharacter = NewCharacter;
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetSelectedCharacter(%s) End"), *StaticEnum<ECharacterType>()->GetValueAsString(NewCharacter));
}

void ASTPlayerState::SetCurrentHP(const float NewHP)
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetCurrentHP(%.1f) Start"), NewHP);

	PlayerStateInfo.CurrentHP = NewHP;
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetCurrentHP(%.1f) End"), NewHP);
}

void ASTPlayerState::SubtractCurrentHP(const float Amount)
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SubtractCurrentHP(%.1f) Start"), Amount);

	PlayerStateInfo.CurrentHP -= Amount;
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SubtractCurrentHP(%.1f) End"), Amount);
}

void ASTPlayerState::SetMaxHP(const float NewHP)
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetMaxHP(%.1f) Start"), NewHP);

	PlayerStateInfo.MaxHP = NewHP;
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetMaxHP(%.1f) End"), NewHP);
}

void ASTPlayerState::SetCurrentAmmo(const int32 NewCurrentAmmo)
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetCurrentAmmo(%.d) Start"), NewCurrentAmmo);

	PlayerStateInfo.CurrentAmmo = NewCurrentAmmo;
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetCurrentAmmo(%.d) End"), NewCurrentAmmo);
}

void ASTPlayerState::SubtractCurrentAmmo()
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SubtractCurrentAmmo() Start"));

	--PlayerStateInfo.CurrentAmmo;
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SubtractCurrentAmmo() End"));
}

void ASTPlayerState::SetMaxAmmo(const int32 NewMaxAmmo)
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetMaxAmmo(%d) Start"), NewMaxAmmo);

	PlayerStateInfo.MaxAmmo = NewMaxAmmo;
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetMaxAmmo(%d) End"), NewMaxAmmo);
}

void ASTPlayerState::AddKillCount()
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::AddKillCount() Start : KillCount(%d)"), PlayerStateInfo.KillCount);

	++PlayerStateInfo.KillCount;
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::AddKillCount() End : KillCount(%d)"), PlayerStateInfo.KillCount);
}

void ASTPlayerState::SetKillCount(const int32 NewKillCount)
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetKillCount(%d) Start"), NewKillCount);

	PlayerStateInfo.KillCount = NewKillCount;
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetKillCount(%d) End"), NewKillCount);
}

void ASTPlayerState::AddScore(const int32 Amount)
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::AddScore(%d) Start : Score(%d)"), Amount, PlayerStateInfo.Score);
	
	PlayerStateInfo.Score += Amount;
	if (PlayerStateInfo.Score > PlayerStateInfo.HighScore)	// 최고기록 갱신시 같이 업데이트
		SetHighScore(PlayerStateInfo.Score);
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::AddScore(%d) End : Score(%d)"), Amount, PlayerStateInfo.Score);
}

void ASTPlayerState::SetScore(const int32 NewScore)
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::AddScore(%d) Start"), NewScore);

	PlayerStateInfo.Score += NewScore;
	if (PlayerStateInfo.Score > PlayerStateInfo.HighScore)	// 최고기록 갱신시 같이 업데이트
		SetHighScore(PlayerStateInfo.Score);
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::AddScore(%d) End"), NewScore);
}

void ASTPlayerState::SetHighScore(const int32 NewHighScore)	// 이건 호출할 필요 없음
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetHighScore(%d) Start"), NewHighScore);

	PlayerStateInfo.HighScore = NewHighScore;
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetHighScore(%d) End"), NewHighScore);
}

void ASTPlayerState::SetCurrWeaponName(const FString& WeaponName)
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetCurrWeaponName(%s) Start"), *WeaponName);

	PlayerStateInfo.CurrWeaponName = WeaponName;
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetCurrWeaponName(%s) End"), *WeaponName);
}

/********** protected *************/
void ASTPlayerState::BeginPlay()
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::BeginPlay() Start"));
	Super::BeginPlay();

	// 레벨 로드시 Game Instance에 저장된 정보 불러오기
	if (USTGameInstance* STGameInstance = Cast<USTGameInstance>(GetGameInstance()))
	{
		PlayerStateInfo = STGameInstance->GetPlayerStateInfo();
	}
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::BeginPlay() KillCount(%d)"), PlayerStateInfo.KillCount);
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::BeginPlay() End"));
}
