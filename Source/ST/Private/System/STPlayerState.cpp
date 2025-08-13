#include "System/STPlayerState.h"

#include "Enemy/STEnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/STHealthComponent.h"
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

	AddTotalDamageReceived(DamageAmount);
	
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

	int32 UsedAmmo = PlayerStateInfo.CurrentAmmo - CurrentAmmo;
	AddTotalUsedAmmo(UsedAmmo);		// 보통 1발씩 증가
	
	SetCurrentAmmo(CurrentAmmo);
	SetMaxAmmo(MaxAmmo);
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::OnAmmoChanged(%d / %d) End"), CurrentAmmo, MaxAmmo);
}

// GameMode::EndStage에서 직접 호출
void ASTPlayerState::CalculateScore()
{
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::CalculateScore() Start"));

	int32 RemainingTime = 0;
	if (ASTGameState* STGameState = Cast<ASTGameState>(GetWorld()->GetGameState()))
	{
		RemainingTime = STGameState->GetGameStateInfo().RemainingTime;
	}
	else
	{
		UE_LOG(LogSystem, Warning, TEXT("ASTPlayerState::CalculateScore() Can't Load STGameState->GameStateInfo.RemainingTime"));
	}

	int32 NewScore = (RemainingTime * PlayerStateInfo.KillCount * PlayerStateInfo.TotalDamageInflicted)
							- (PlayerStateInfo.TotalDamageInflicted * PlayerStateInfo.TotalUsedAmmo * ScoreMultiplier);

	SetScore(PlayerStateInfo.Score + NewScore);	// 최고기록 갱신시 내부에서 업데이트 함
	
	UE_LOG(LogSystem, Log, TEXT("ASTPlayerState::CalculateScore() End"));
}