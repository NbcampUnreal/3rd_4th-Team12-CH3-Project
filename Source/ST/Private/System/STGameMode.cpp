#include "System/STGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "System/STGameState.h"
#include "Enemy/STEnemyBase.h"
#include "Player/STHealthComponent.h"
#include "Player/STPlayerCharacter.h"
#include "System/StageClearZone.h"
#include "System/StageInfoData.h"
#include "System/STGameInstance.h"
#include "System/STLog.h"
#include "System/STPlayerState.h"
#include "UI/STStagePlayerController.h"

/************** public functions **************/
ASTGameMode::ASTGameMode()
{
	/* 기본 클래스 설정 */
	GameStateClass = ASTGameState::StaticClass();
	PlayerControllerClass = ASTStagePlayerController::StaticClass();
	DefaultPawnClass = nullptr;

	/* 변수 기본값 설정 */
	TotalEnemies = 0;
	DeadEnemies = 0;
	StageTimeLimit = 123;
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
	BindPlayerDeathEvent();
	ResetStage();
	StartStage();
	
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::BeginPlay() End"));
}

void ASTGameMode::RestartPlayer(AController* NewPlayer)
{
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::RestartPlayer() Start"));

	USTGameInstance* STGameInstance = Cast<USTGameInstance>(GetGameInstance());
	if (!STGameInstance)
	{
		Super::RestartPlayer(NewPlayer);
		return;
	}

	if (!STGameInstance->PlayerPawnInstance)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		STGameInstance->PlayerPawnInstance = GetWorld()->SpawnActor<APawn>(STGameInstance->MainPlayerClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	}

	APawn* PlayerPawnInstance = STGameInstance->PlayerPawnInstance;
	if (!PlayerPawnInstance)
	{
		UE_LOG(LogSystem, Error, TEXT("ASTGameMode::RestartPlayer() PlayerpawnInstance is nullptr in GameInstance!"));
		Super::RestartPlayer(NewPlayer);
		return;
	}

	AActor* PlayerStart = FindPlayerStart(NewPlayer, "");
	if (PlayerStart)
	{
		PlayerPawnInstance->SetActorLocationAndRotation(PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation());
	}
	else
	{
		UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::RestartPlayer() No PlayerStart found. Spawning at default location."));
		PlayerPawnInstance->SetActorLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
	}

	// 캐릭터의 상태를 활성화
	PlayerPawnInstance->SetActorHiddenInGame(false);
	PlayerPawnInstance->SetActorEnableCollision(true);
	PlayerPawnInstance->SetActorTickEnabled(true);

	// PlayerController가 캐릭터를 소유
	NewPlayer->Possess(PlayerPawnInstance);

	// 스켈레탈 메시 교체 로직
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::RestartPlayer() Character Type : %s"), *StaticEnum<ECharacterType>()->GetValueAsString(STGameInstance->SelectedCharacter));
	ASTPlayerCharacter* PlayerCharacter = Cast<ASTPlayerCharacter>(PlayerPawnInstance);
	if (PlayerCharacter)
	{
		USkeletalMesh* MeshToUse = nullptr;
		if (STGameInstance->SelectedCharacter == ECharacterType::JaxMercer)
		{
			UE_LOG(LogSystem, Log, TEXT("ASTGameMode::RestartPlayer() Use Jax mercer Mesh"));
			MeshToUse = STGameInstance->JaxMercerCharacterMesh;
		}
		else if (STGameInstance->SelectedCharacter == ECharacterType::AvaRaines)
		{
			UE_LOG(LogSystem, Log, TEXT("ASTGameMode::RestartPlayer() Use AvaRaines Mesh"));
			MeshToUse = STGameInstance->AvaRainesCharacterMesh;
		}

		if (MeshToUse)
		{
			PlayerCharacter->GetMesh()->SetSkeletalMesh(MeshToUse);
		}
		else
		{
			UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::PostLogin() No Mesh To Use!"));
		}
	}

	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::RestartPlayer() End"));
}


/************** private functions **************/
void ASTGameMode::StartStage()
{
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::StartStage() Start"));

	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::StartStage() Stage Started. Time Limit : %d seconds"), StageTimeLimit);
	GetWorldTimerManager().SetTimer(StageTimerHandle, this, &ASTGameMode::OnTimeOver, StageTimeLimit, false);		// StageTimeLimit 후 OnTimeOver실행
	GetWorldTimerManager().SetTimer(StageTimerUpdateHandle, this, &ASTGameMode::UpdateStageTimerUI, 1.0f, true);	// 1초마다 UI 업데이트
	
	SetStagePhase(EStagePhase::InProgress);
	
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::StartStage() End"));
}

void ASTGameMode::EndStage(const EStageResult Result)
{
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::EndStage(%s) Start"), *UEnum::GetValueAsString(Result));
	
	GetWorldTimerManager().ClearTimer(StageTimerHandle);
	GetWorldTimerManager().ClearTimer(StageTimerUpdateHandle);
	
	if (ASTGameState* STGameState = GetGameState<ASTGameState>())
	{
		STGameState->SetStageResult(Result);
	}
	
	ASTStagePlayerController* STPlayerController = Cast<ASTStagePlayerController>(GetWorld()->GetFirstPlayerController());
	if (ASTPlayerState* STPlayerState = STPlayerController->GetPlayerState<ASTPlayerState>())
	{
		if (Result == EStageResult::Clear)	STPlayerState->CalculateScore(true);
		else if (Result == EStageResult::Fail)	STPlayerState->CalculateScore(false);
	}
	else
	{
		UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::EndStage() Can't Get ASTPlayerState"));
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
	FStageInfoRow* StageInfo = GetStageInfoFromDataTable(CurrentStageName);
	StageTimeLimit = StageInfo->TimeLimit;

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
		STGameState->SetStageGoalText(StageInfo->StageGoalText);
		STGameState->SetStageProgressList(StageInfo->StageProgressList);
		/*UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::ResetStage() StageProgressList[0] : %s"), *StageInfo->StageProgressList[0].ToString());
		UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::ResetStage() StageProgressList[1] : %s"), *StageInfo->StageProgressList[1].ToString());*/
	}
	
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::ResetStage() End"));
}

void ASTGameMode::OnTimeOver()
{
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::OnTimeOver() Start"));

	if (ASTGameState* STGameState = GetGameState<ASTGameState>())
	{
		STGameState->SetRemainingTime(0);
	}
	
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

FStageInfoRow* ASTGameMode::GetStageInfoFromDataTable(const FString& StageName) const
{
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::GetStageInfoFromDataTable(%s) Start"), *StageName);

	if (!StageInfoTable)
	{
		UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::GetStageInfoFromDataTable() No Stage Info Table"));
		return nullptr;
	}
	
	if (FStageInfoRow* Row = StageInfoTable->FindRow<FStageInfoRow>(FName(*StageName), TEXT("")))
	{
		UE_LOG(LogSystem, Log, TEXT("ASTGameMode::GetStageInfoFromDataTable(%s) End"), *StageName);
		return Row;
	}
	
	UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::GetStageInfoFromDataTable(%s) Can not find row for Stage in StageInfoTable"), *StageName);
	return nullptr;
}

void ASTGameMode::BindStageClearZoneEnterEvent()
{
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::BindStageClearZoneEnterEvent() Start"));
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStageClearZone::StaticClass(),FoundActors);

	if (FoundActors.Num() > 0)
	{
		if (AStageClearZone* ClearZone = Cast<AStageClearZone>(FoundActors[0]))
		{
			ClearZone->OnPlayerEnteredClearZone.AddDynamic(this, &ASTGameMode::ASTGameMode::HandlePlayerEnteredClearZone);
		}
	}
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::BindStageClearZoneEnterEvent() End"));
}

void ASTGameMode::BindPlayerDeathEvent()
{
	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::BindPlayerDeathEvent() Start"));

	if (ASTPlayerCharacter* STPlayerCharacter = Cast<ASTPlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn()))
	{
		if (USTHealthComponent* HealthComponent = STPlayerCharacter->GetHealthComponent())
		{
			HealthComponent->OnCharacterDeath.AddDynamic(this, &ASTGameMode::OnPlayerDied);
			UE_LOG(LogSystem, Log, TEXT("ASTGameMode::BindPlayerDeathEvent() Death Event Bind"));
		}
		else
		{
			UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::BindPlayerDeathEvent() Can't Death Event Bind"));
		}
	}
	else
	{
		UE_LOG(LogSystem, Warning, TEXT("ASTGameMode::BindPlayerDeathEvent() Can't Death Event Bind"));
	}

	UE_LOG(LogSystem, Log, TEXT("ASTGameMode::BindPlayerDeathEvent() End"));
}

void ASTGameMode::UpdateStageTimerUI()
{
	// UE_LOG(LogSystem, Log, TEXT("ASTGameMode::UpdateStageTimerUI() Start"));		// 1초마다 반복으로 로그가 너무 많아서 비활성화

	if (ASTGameState* STGameState = GetGameState<ASTGameState>())
	{
		float RemainingTimeFloat = GetWorld()->GetTimerManager().GetTimerRemaining(StageTimerHandle);	// 내림(0~29초), 올림(1~30초)
		int32 RemainingTimeSeconds = FMath::CeilToInt(RemainingTimeFloat);
		STGameState->SetRemainingTime(RemainingTimeSeconds);
	}
	
	// UE_LOG(LogSystem, Log, TEXT("ASTGameMode::UpdateStageTimerUI() Start"));
}
