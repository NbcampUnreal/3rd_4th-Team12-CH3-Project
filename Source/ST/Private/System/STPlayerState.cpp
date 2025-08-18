#include "System/STPlayerState.h"

#include "Enemy/STEnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/STHealthComponent.h"
#include "Player/STPlayerBaseData.h"
#include "Player/STPlayerCharacter.h"
#include "Player/STWeaponManagerComponent.h"
#include "System/STGameInstance.h"
#include "System/STGameState.h"
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

void ASTPlayerState::OnChangedCurrentWeapon(const TSoftClassPtr<ASTWeaponBase> NewWeapon)
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetCurrentAmmo(%.s) Start"), *NewWeapon->GetName());

	PlayerStateInfo.PlayerWeaponData.WeaponClass = NewWeapon;
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetCurrentAmmo(%.s) End"), *NewWeapon->GetName());
	
}

void ASTPlayerState::SetCurrentAmmo(const int32 NewCurrentAmmo)
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetCurrentAmmo(%.d) Start"), NewCurrentAmmo);

	PlayerStateInfo.PlayerWeaponData.CurrentAmmo = NewCurrentAmmo;
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetCurrentAmmo(%.d) End"), NewCurrentAmmo);
}

void ASTPlayerState::SubtractCurrentAmmo()
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SubtractCurrentAmmo() Start"));

	--PlayerStateInfo.PlayerWeaponData.CurrentAmmo;
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SubtractCurrentAmmo() End"));
}

void ASTPlayerState::SetMaxAmmo(const int32 NewMaxAmmo)
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetMaxAmmo(%d) Start"), NewMaxAmmo);

	PlayerStateInfo.PlayerWeaponData.MaxAmmo = NewMaxAmmo;
	
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

	PlayerStateInfo.Score = NewScore;
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

	//PlayerStateInfo.PlayerWeaponData.WeaponClass->weapo = WeaponName;
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::SetCurrWeaponName(%s) End"), *WeaponName);
}

void ASTPlayerState::AddTotalDamageReceived(const float Amount)
{
	PlayerStateInfo.TotalDamageReceived += Amount;
}

void ASTPlayerState::AddTotalDamageInflicted(const float Amount)
{
	PlayerStateInfo.TotalDamageInflicted += Amount;
}

void ASTPlayerState::AddTotalUsedAmmo(const int32 Amount)
{
	// UE_LOG(LogSystem, Warning, TEXT("AddTotalUsed AMMO : %d"), PlayerStateInfo.TotalUsedAmmo);
	PlayerStateInfo.TotalUsedAmmo += Amount;
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


	// 이벤트 바인딩
	if (AController* Controller = GetOwner<APlayerController>())
	{
		if (ASTPlayerCharacter* STPlayerCharacter = Cast<ASTPlayerCharacter>(Controller->GetPawn()))
		{
			if (USTHealthComponent* HealthComponent = STPlayerCharacter->GetHealthComponent())
			{
				// HealthComponent의 델리게이트에 PlayerState의 함수를 바인딩합니다.
				HealthComponent->OnHealthChanged.AddDynamic(this, &ASTPlayerState::OnHealthChanged);
				UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::BeginPlay() - Delegates Bound to HealthComponent"));
			}
			if (USTWeaponManagerComponent* WeaponManagerComponent = STPlayerCharacter->FindComponentByClass<USTWeaponManagerComponent>())
			{
				WeaponManagerComponent->AmmoChangeDelegate.AddDynamic(this, &ASTPlayerState::OnAmmoChanged);
				WeaponManagerComponent->EquipChangedDelegate.AddDynamic(this, &ASTPlayerState::OnChangedCurrentWeapon);
				
				// TODO: 장비 장착시 이름 가져오기 // WeaponManagerComponent->EquipDelegate.AddDynamic(this, &ASTPlayerState::OnEquipWeapon); 
			}
		}
	}

	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASTEnemyBase::StaticClass(), Enemies);
	for (AActor* E : Enemies)
	{
		if (ASTEnemyBase* Enemy = Cast<ASTEnemyBase>(E))
		{
			Enemy->OnDamageTaken.AddDynamic(this, &ASTPlayerState::OnDamageTaken);
		}
	}
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::BeginPlay() End"));
}

// Enemy에게 데미지를 입힐 때 호출
void ASTPlayerState::OnDamageTaken(AActor* DamagedActor, float DamageAmount, bool bCritical)
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::OnDamageTaken(%f / isCritical(%d) Start"), DamageAmount, bCritical);

	AddScore(DamageAmount);					// JM : 바로 점수에 더해버리기
	AddTotalDamageInflicted(DamageAmount);	// JM : 그럼 이걸 유지할 필요가..? 통계?
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::OnDamageTaken(%f / isCritical(%d) End"), DamageAmount, bCritical);
}



// Player 체력이 변할 때 호출
void ASTPlayerState::OnHealthChanged(float CurrentHP, float MaxHP)
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::OnHealthChanged(%f / %f) Start"), CurrentHP, MaxHP);

	float DamageReceived = PlayerStateInfo.CurrentHP - CurrentHP;
	if (DamageReceived > 0)	// 체력이 회복되어도 받은 데미지가 감소하지 않음
	{
		AddTotalDamageReceived(DamageReceived);
	}
	
	SetCurrentHP(CurrentHP);
	SetMaxHP(MaxHP);	// 굳이...?
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::OnHealthChanged(%f / %f) End"), CurrentHP, MaxHP);
}

// 총알 발사시 호출
void ASTPlayerState::OnAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo)
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::OnAmmoChanged(%d / %d) Start"), CurrentAmmo, MaxAmmo);

	// int32 UsedAmmo = PlayerStateInfo.PlayerWeaponData.CurrentAmmo - CurrentAmmo;
	AddTotalUsedAmmo(1);		// 보통 1발씩 증가
	
	SetCurrentAmmo(CurrentAmmo);
	SetMaxAmmo(MaxAmmo);
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::OnAmmoChanged(%d / %d) End"), CurrentAmmo, MaxAmmo);
}



// 매 초 마다 이벤트 수신(OnRemainingTimeUpdate)
/*void ASTPlayerState::CalculateScore(int32 NewRemainingTime)
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::CalculateScore() Start"));

	

	

	int32 RemainingTime = NewRemainingTime;
	/*if (ASTGameState* STGameState = Cast<ASTGameState>(GetWorld()->GetGameState()))
	{
		RemainingTime = STGameState->GetGameStateInfo().RemainingTime;
	}
	else
	{
		UE_LOG(LogSystem, Warning, TEXT("ASTPlayerState::CalculateScore() Can't Load STGameState->GameStateInfo.RemainingTime"));
	}#1#

	const int32 Time = FMath::Clamp(RemainingTime, 0, RemainingTime);
	const float Attack = PlayerStateInfo.TotalDamageInflicted;
	const float Damage = PlayerStateInfo.TotalDamageReceived;
	const int32 Ammo = PlayerStateInfo.TotalUsedAmmo;

	int32 NewScore = static_cast<int32>((Time * 5) + (Attack * 1) - (Damage * 1) - (Ammo * 0.1f));
	UE_LOG(LogSystem, Warning, TEXT("ASTPlayerState::CalculateScore() NewScore(%d) = Score(%d) + Time(%d)*5 + Attack(%f)*0.1f - Damage(%f)*1 - Ammo(%d)*0.1"), NewScore, PlayerStateInfo.Score, Time, Attack, Damage, Ammo);
	// 남은시간 초당 5점, 입힌 피해량 10당 1점, 받은 데미지 1당 -1점, 사용한 총알 수 10발당 -1점
	NewScore = FMath::Clamp(NewScore, 0, NewScore);

	SetScore(NewScore);	// 최고기록 갱신시 내부에서 업데이트 함
	// SetScore(PlayerStateInfo.Score + NewScore);	// 최고기록 갱신시 내부에서 업데이트 함
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::CalculateScore() End"));
}*/

void ASTPlayerState::CalculateScore(bool bIsStageClear)
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::CalculateScoreAtGameOver() Start"));
	
	int32 RemainingTime = 0;
	if (ASTGameState* STGameState = Cast<ASTGameState>(GetWorld()->GetGameState()))
	{
		RemainingTime = STGameState->GetGameStateInfo().RemainingTime;
	}
	else
	{
		UE_LOG(LogSystem, Warning, TEXT("ASTPlayerState::CalculateScoreAtGameOver() Can't Load STGameState->GameStateInfo.RemainingTime"));
	}

	int32 NewScore = PlayerStateInfo.Score - PlayerStateInfo.TotalDamageReceived * 1 - PlayerStateInfo.TotalUsedAmmo * 0.1f;
	UE_LOG(LogSystem, Warning, TEXT("ASTPlayerState::CalculateScoreAtGameOver() %d = Score(%d) - %.1f - %f"), NewScore, PlayerStateInfo.Score, PlayerStateInfo.TotalDamageReceived, PlayerStateInfo.TotalUsedAmmo * 0.1f);
	if (bIsStageClear)
	{
		constexpr int32 ClearBonus = 1000;
		NewScore += (RemainingTime * 5 + ClearBonus);	
	}
	SetScore(FMath::Clamp(NewScore, 0, NewScore));

	if (USTGameInstance* STGameInstance = Cast<USTGameInstance>(GetWorld()->GetGameInstance()))
	{
		FSaveData SaveData;
		SaveData.HighScore = PlayerStateInfo.HighScore;
		STGameInstance->SaveSavedData(SaveData);
	}
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::CalculateScoreAtGameOver() End"));
}
