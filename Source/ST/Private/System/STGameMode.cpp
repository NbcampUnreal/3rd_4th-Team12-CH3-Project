#include "System/STGameMode.h"

#include "ToolContextInterfaces.h"
#include "Kismet/GameplayStatics.h"
#include "System/STGameState.h"
#include "Enemy/STEnemyBase.h"
#include "System/StageClearZone.h"
#include "System/StageInfoData.h"
#include "System/STLog.h"
#include "System/STPlayerState.h"
#include "UI/STStagePlayerController.h"

/************** public functions **************/
ASTGameMode::ASTGameMode()
{
	/* 기본 클래스 설정 */
	GameStateClass = ASTGameState::StaticClass();
	PlayerControllerClass = ASTStagePlayerController::StaticClass();

	/* 변수 기본값 설정 */
	TotalEnemies = 0;
	DeadEnemies = 0;
	StageTimeLimit = 300.0f;
	bStageCleared = false;
	StageInfoTable = nullptr;
	/*RemainingTime = StageTimeLimit;
	BossPhase = 1;*/
}

void ASTGameMode::OnEnemyKilled()
{
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::OnEnemyKilled() Start"));

	DeadEnemies++;
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::OnEnemyKilled() Enemy killed(%d / %d)"), DeadEnemies, TotalEnemies);

	// STPlayerState Kill Count 증가
	ASTStagePlayerController* STPlayerController = Cast<ASTStagePlayerController>(GetWorld()->GetFirstPlayerController());
	if (ASTPlayerState* STPlayerState = STPlayerController->GetPlayerState<ASTPlayerState>())
	{
		STPlayerState->AddKillCount();
	}
	
	if (ASTGameState* STGameState = GetGameState<ASTGameState>())
	{
		STGameState->SetRemainingEnemies(TotalEnemies - DeadEnemies);
	}

	if (DeadEnemies >= TotalEnemies)
	{
		// TODO: 모든 적을 섬멸했다. 다음 스테이지로 이동하라 메시지 표시 (이벤트? OnEnemyAllKilled?)
		UE_LOG(LogSystem, Log, TEXT("ASTGameMode::OnEnemyKilled() All Enemies Defeated. Proceed to the Clear Zone"));
	}
	
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::OnEnemyKilled() End"));
}

void ASTGameMode::OnPlayerDied()
{
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::OnPlayerDied() Start"));
	
	SetStagePhase(EStagePhase::Fail);
	EndStage(EStageResult::Fail);
	
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::OnPlayerDied() End"));
}

/************** protected functions **************/
void ASTGameMode::BeginPlay()
{
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::BeginPlay() Start"));
	
	Super::BeginPlay();
	BindStageClearZoneEnterEvent();
	ResetStage();
	StartStage();
	
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::BeginPlay() End"));
}

/************** private functions **************/
void ASTGameMode::StartStage()
{
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::StartStage() Start"));

	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::StartStage() Stage Started. Time Limit : %.f seconds"), StageTimeLimit);
	GetWorldTimerManager().SetTimer(StageTimerHandle, this, &ASTGameMode::OnTimeOver, StageTimeLimit, false);
	SetStagePhase(EStagePhase::InProgress);
	
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::StartStage() End"));
}

void ASTGameMode::EndStage(const EStageResult Result)
{
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::EndStage(%s) Start"), *UEnum::GetValueAsString(Result));
	
	GetWorldTimerManager().ClearTimer(StageTimerHandle);
	if (ASTGameState* STGameState = GetGameState<ASTGameState>())
	{
		STGameState->SetStageResult(Result);
	}

	if (Result == EStageResult::Clear)
	{
		OnStageClear.Broadcast();
	}
	else if (Result == EStageResult::Fail)
	{
		OnStageFailed.Broadcast();
	}
	else
	{
		UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::EndStage() Result == NONE"));
	}
	
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::EndStage(%s) End"), *UEnum::GetValueAsString(Result));
}

void ASTGameMode::ResetStage()
{
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::ResetStage() Start"));

	DeadEnemies = 0;
	bStageCleared = false;

	FString CurrentStageName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	StageTimeLimit = GetStageInfoFromDataTable(CurrentStageName);

	TArray<AActor*> EnemyActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASTEnemyBase::StaticClass(), EnemyActors);
	TotalEnemies = EnemyActors.Num();
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::ResetStage() Stage(%s) > %d Enemies Found"), *CurrentStageName, TotalEnemies);

	if (ASTGameState* STGameState = GetGameState<ASTGameState>())
	{
		STGameState->SetStagePhase(EStagePhase::Start);
		STGameState->SetRemainingEnemies(TotalEnemies);
		STGameState->SetRemainingTime(StageTimeLimit);
		STGameState->SetStageResult(EStageResult::None);
		STGameState->SetBossPhase(1);
	}
	
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::ResetStage() End"));
}

void ASTGameMode::OnTimeOver()
{
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::OnTimeOver() Start"));

	SetStagePhase(EStagePhase::Fail);
	EndStage(EStageResult::Fail);
	
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::OnTimeOver() End"));
}

void ASTGameMode::HandlePlayerEnteredClearZone()
{
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::HandlePlayerEnteredClearZone() Start"));

	if (DeadEnemies >= TotalEnemies && !bStageCleared)
	{
		UE_LOG(LogSystem, Log, TEXT("ASTGameMode::HandlePlayerEnteredClearZone() Stage Clear Condition"));
		bStageCleared = true;
		SetStagePhase(EStagePhase::Completed);
		EndStage(EStageResult::Clear);
	}
	else if (DeadEnemies < TotalEnemies)
	{
		UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::HandlePlayerEnteredClearZone() Not Yet Clear Condition(%d/%d enemies killed)"), DeadEnemies, TotalEnemies);
		// TODO: 아직 조건이 되지 않았다는 UI 메시지 표시
	}
	else if (bStageCleared)
	{
		UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::HandlePlayerEnteredClearZone() Already Stage Cleared)"));
	}
	
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::HandlePlayerEnteredClearZone() End"));
}

void ASTGameMode::SetStagePhase(const EStagePhase NewPhase) const
{
	if (ASTGameState* STGameState = GetGameState<ASTGameState>())
	{
		STGameState->SetStagePhase(NewPhase);
	}
}

float ASTGameMode::GetStageInfoFromDataTable(const FString& StageName) const
{
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::GetStageInfoFromDataTable(%s) Start"), *StageName);

	if (!StageInfoTable)
	{
		UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::GetStageInfoFromDataTable() No Stage Info Table"));
		return 300.0f;
	}
	
	if (const FStageInfoRow* Row = StageInfoTable->FindRow<FStageInfoRow>(FName(*StageName), TEXT("")))
	{
		UE_LOG(LogSystem, Log, TEXT("ASTGameMode::GetStageInfoFromDataTable(%s) End"), *StageName);
		return Row->TimeLimit;
	}
	
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::GetStageInfoFromDataTable(%s) Can not find row for Stage in StageInfoTable"), *StageName);
	return 300.0f;
	
	// TODO: 향후 스테이지 정보 많아지면 구조체로 확장해서 받아오기
}

void ASTGameMode::BindStageClearZoneEnterEvent()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStageClearZone::StaticClass(),FoundActors);

	if (FoundActors.Num() > 0)
	{
		if (AStageClearZone* ClearZone = Cast<AStageClearZone>(FoundActors[0]))
		{
			ClearZone->OnPlayerEnteredClearZone.AddDynamic(this, &ASTGameMode::ASTGameMode::HandlePlayerEnteredClearZone);
		}
	}
}

/*
void ASTGameMode::BeginPlay()
{
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::BeginPlay() Start"));
	Super::BeginPlay();

	OnPlayerEnteredClearZone.AddDynamic(this, &ASTGameMode::HandlePlayerEnteredClearZone);	// Delegate에 함수 바인딩

	FString CurrentStageName = UGameplayStatics::GetCurrentLevelName(GetWorld());	// TimeLimit을 csv 파일로 관리
	StageTimeLimit = GetStageTimeLimit(CurrentStageName);
	RemainingTime = StageTimeLimit;

	SetStagePhase(EStagePhase::Start);
	
	ResetStage();
	StartStage();
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::BeginPlay() End"));
}

void ASTGameMode::StartStage()
{
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::StartStage() Start"));
	// 1회성 타이머(루프X)로 제한시간 후 자동 GameOver
	GetWorldTimerManager().SetTimer(StageTimerHandle, this, &ASTGameMode::OnTimeOver, StageTimeLimit, false);

	SetStagePhase(EStagePhase::InProgress);
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::StartStage() End"));
}

void ASTGameMode::OnEnemyKilled()
{
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::OnEnemyKilled() Start"));
	DeadEnemies++;
	if (ASTGameState* STGameState = GetGameState<ASTGameState>())
		STGameState->SetRemainingEnemies(TotalEnemies - DeadEnemies);
	
	// TODO: 모든 적을 죽인 경우에 UI로 특정 위치로 이동하세요 라는 메시지 주기 (특정위치에 파티클 등으로 강조하고)
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::OnEnemyKilled() End"));
}

void ASTGameMode::OnPlayerDied()
{
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::OnPlayerDied() Start"));
	SetStagePhase(EStagePhase::Fail);
	EndStage(EStageResult::Fail);
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::OnPlayerDied() End"));
}

void ASTGameMode::SetBossPhase(int32 NewPhase)
{
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::SetBossPhase() Start"));
	BossPhase = NewPhase;
	if (ASTGameState* STGameState = GetGameState<ASTGameState>())
		STGameState->SetBossPhase(BossPhase);
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::SetBossPhase() End"));
}

void ASTGameMode::OnTimeOver()
{
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::OnTimeOver() Start"));
	SetStagePhase(EStagePhase::Fail);
	EndStage(EStageResult::Fail);
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::OnTimeOver() End"));
}

float ASTGameMode::GetRemainingTime() const
{
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::GetRemainingTime() Start"));
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::GetRemainingTime() End"));
	return GetWorldTimerManager().GetTimerRemaining(StageTimerHandle);
}


void ASTGameMode::EndStage(EStageResult Result)
{
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::EndStage() Start"));
	GetWorldTimerManager().ClearTimer(StageTimerHandle);
	// 여기서 레벨 전환 등 처리는 UI/블루프린트에서
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::EndStage() End"));
}

// NOTE: HandlePlayerEnteredClearZone delegate 연결로 사용 안하는 중
void ASTGameMode::CheckStageClear()
{
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::CheckStageClear() Start"));
	
	if (DeadEnemies >= TotalEnemies)
	{
		SetStagePhase(EStagePhase::Completed);
		EndStage(EStageResult::Clear);
	}
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::CheckStageClear() End"));
}

void ASTGameMode::ResetStage()
{
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::ResetStage() Start"));
	DeadEnemies = 0;
	BossPhase = 1;

	TArray<AActor*> EnemyActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASTEnemyBase::StaticClass(),EnemyActors);
	TotalEnemies = EnemyActors.Num();
	RemainingTime = StageTimeLimit;

	/* 상태값 state에 전달 #1#
	if (ASTGameState* STGameState = GetGameState<ASTGameState>())
	{
		STGameState->SetStagePhase(EStagePhase::Start);
		STGameState->SetBossPhase(BossPhase);
		STGameState->SetRemainingEnemies(TotalEnemies - DeadEnemies);
		STGameState->SetRemainingTime(RemainingTime);
		STGameState->SetStageResult(EStageResult::None);
	}
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::ResetStage() End"));
}

float ASTGameMode::GetStageTimeLimit(const FString StageName) const
{
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::GetStageTimeLimit() Start"));
	if (!StageTimeLimit) return 10.0f; // 기본값 (300.0f), 테스트 (30.0f)

	FStageTimeLimitRow* Row = StageTimeTable->FindRow<FStageTimeLimitRow>(*StageName, TEXT("LookupStageTimeLimit"));
	if (Row)
	{
		UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::GetStageTimeLimit() End"));
		return Row->TimeLimit;
	}
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::GetStageTimeLimit() End"));
	return 10.0f;
}

void ASTGameMode::HandlePlayerEnteredClearZone()
{
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::HandlePlayerEnteredClearZone() Start"));
	if (DeadEnemies >= TotalEnemies && !bStageCleared)
	{
		UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::HandlePlayerEnteredClearZone() Stage Clear Broadcast"));
		bStageCleared = true;

		// TODO: 게임 정보 GI에 저장.. 은 PlayerController에서 하는게 맞으려나?
		OnStageClear.Broadcast();
		
		SetStagePhase(EStagePhase::Completed);
		EndStage(EStageResult::Clear);

		// 현재 스테이지 정보 가져오기
		/*USTGameInstance* STGameInstance = GetGameInstance<USTGameInstance>();
		if (!STGameInstance) return;

		EStageType NextStage = EStageType::None;
		UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::HandlePlayerEnteredClearZone() LastStage : %s"), *StaticEnum<EStageType>()->GetValueAsString(STGameInstance->LastStage));
		switch (STGameInstance->LastStage)
		{
			case EStageType::Stage1:
				NextStage = EStageType::Stage2;
				break;
			case EStageType::Stage2:
				NextStage = EStageType::Stage3;
				break;
			case EStageType::Stage3:
				// TODO: 엔딩으로 이동하거나 따로 처리하기
				// NextStage = EStageType::Ending;
				break;
			default:
				break;
		}

		

		if (NextStage != EStageType::None)
		{
			STGameInstance->LastStage = NextStage;		
			STGameInstance->GoToLevel(NextStage);	// 레벨 전환 함수
		}
		else
		{
			// TODO: 엔딩처리 또는 결과화면이 뜨게 하기
			// STGameInstance->GoToLevel(EStage::Ending); 
		}#1#
	}
	
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::HandlePlayerEnteredClearZone() End"));
}

void ASTGameMode::SetStagePhase(EStagePhase NewPhase)
{
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::SetStagePhase() Start"));
	if (ASTGameState* STGameState = GetGameState<ASTGameState>())
		STGameState->SetStagePhase(NewPhase);
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::SetStagePhase() End"));
}
*/
