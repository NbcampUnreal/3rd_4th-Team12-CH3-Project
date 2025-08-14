#include "UI/STStagePlayerController.h"
#include "UI/STStageWidget.h"
#include "UI/STPauseMenuWidget.h"
#include "UI/STScoreboardWidget.h"
#include "UI/STGameOverWidget.h"
#include "UI/STGameClearWidget.h"
#include "UI/UWCrosshairWidget.h"            
#include "Player/STMovementComponent.h"      
#include "Player/STPlayerCharacter.h"        
#include "Blueprint/UserWidget.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "System/STGameInstance.h"
#include "System/STGameTypes.h"
#include "Enemy/STEnemyBase.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/STHealthComponent.h"
#include "Player/STPlayerCharacter.h"
#include "System/STGameMode.h"
#include "System/STGameState.h"
#include "System/STLog.h"
#include "System/STPlayerState.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/LevelScriptActor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/STWeaponManagerComponent.h"

ASTStagePlayerController::ASTStagePlayerController()
{
	bShowMouseCursor = false;
	PrimaryActorTick.bCanEverTick = true;
	
	static ConstructorHelpers::FClassFinder<USTScoreboardWidget> WidgetClass(TEXT("/Game/UI/UI_BP/BP_ScoreboardUI"));
	if (WidgetClass.Succeeded())
	{
		ScoreboardWidgetClass = WidgetClass.Class;
	}
	
	// 게임 오버 UI 위젯 연결
	static ConstructorHelpers::FClassFinder<USTGameOverWidget> GameOverClass(TEXT("/Game/UI/UI_BP/BP_GameOverUI"));
	if (GameOverClass.Succeeded())
	{
		GameOverWidgetClass = GameOverClass.Class;
	}

	// 게임 클리어 UI 위젯 연결
	static ConstructorHelpers::FClassFinder<USTGameClearWidget> GameClearClass(TEXT("/Game/UI/UI_BP/BP_GameClearUI"));
	if (GameClearClass.Succeeded())
	{
		GameClearWidgetClass = GameClearClass.Class;
	}
}

void ASTStagePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (StageWidgetClass)
	{
		StageWidget = CreateWidget<USTStageWidget>(this, StageWidgetClass);
		if (StageWidget)
		{
			StageWidget->AddToViewport();
		}
	}

	// 점수판 위젯 생성 및 숨김 설정
	if (ScoreboardWidgetClass)
	{
		ScoreboardWidget = CreateWidget<USTScoreboardWidget>(this, ScoreboardWidgetClass);
		if (ScoreboardWidget)
		{
			ScoreboardWidget->AddToViewport();
			ScoreboardWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
	
	// 실제 데이터 대신 임시 값으로 전달
	UpdateEnemyStatus(0, 10); // 적 처치: 0 / 총 10명
	AddDamageKillLog(TEXT("10의 피해를 받았습니다.")); // 로그 메시지

	//게임 클리어 화면 테스트
	//ShowGameClearResult(15000, 20000); // 점수: 15000, 최고기록: 20000


	// JM : GameMode OnStageClear 이벤트 바인딩
	if (ASTGameMode* STGameMode = GetWorld()->GetAuthGameMode<ASTGameMode>())
	{
		STGameMode->OnStageClear.AddDynamic(this, &ASTStagePlayerController::HandleStageClear);
		STGameMode->OnStageFailed.AddDynamic(this, &ASTStagePlayerController::HandleStageFailed);
	}

	
	// JM: 타이머 업데이트 델리게이트 바인딩 (1초마다 호출함)
	if (ASTGameState* STGameState = Cast<ASTGameState>(GetWorld()->GetGameState()))
	{
		STGameState->OnRemainingTimeUpdated.AddDynamic(this, &ASTStagePlayerController::UpdateTimer);

		const int32 InitialRemaining = STGameState->GetGameStateInfo().RemainingTime;
		UpdateTimer(InitialRemaining);
	}

	

	if (ASTPlayerCharacter* PC = Cast<ASTPlayerCharacter>(GetPawn()))
	{
		CachedMoveComp = PC->GetPlayerMovementComponent();
		if (CachedMoveComp)
		{
			bPrevZoomState = CachedMoveComp->IsZooming();

			if (StageWidget)
			{
				if (UWidget* Found = StageWidget->GetWidgetFromName(TEXT("WBP_CrossHair")))
				{
					CachedCrosshair = Cast<UUWCrosshairWidget>(Found);
					if (CachedCrosshair)
					{
						CachedCrosshair->SetZoom(bPrevZoomState);
					}
				}
			}
		}
	}
	
	{
		int32 BoundCount = 0;
		TArray<AActor*> Enemies;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASTEnemyBase::StaticClass(), Enemies);
		for (AActor* E : Enemies)
		{
			if (ASTEnemyBase* Enemy = Cast<ASTEnemyBase>(E))
			{
				Enemy->OnDamageTaken.AddDynamic(this, &ASTStagePlayerController::HandleEnemyDamageTaken);
				Enemy->OnDied.AddDynamic(this, &ASTStagePlayerController::HandleEnemyDied_ShowConfirm);
				++BoundCount;
			}
		}
		// 총 적 수 기록 & UI 반영
		TotalEnemyCount = Enemies.Num();
		KilledEnemyCount = 0; // 스테이지 시작 시 0부터
		UpdateEnemyStatus(KilledEnemyCount, TotalEnemyCount);
	}
	
	//미션 목표 생성
	RefreshMissionProgress(0);
}

void ASTStagePlayerController::SetupInputComponent()
{	
	Super::SetupInputComponent();

	// 테스트용 키 (P)
	InputComponent->BindKey(EKeys::P, IE_Pressed, this, &ASTStagePlayerController::TogglePauseMenu);

	// Tab 키 입력 바인딩
	InputComponent->BindKey(EKeys::Tab, IE_Pressed, this, &ASTStagePlayerController::ShowScoreboard);
	InputComponent->BindKey(EKeys::Tab, IE_Released, this, &ASTStagePlayerController::HideScoreboard);
}

void ASTStagePlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (CachedMoveComp && CachedCrosshair)
	{
		const bool bNow = CachedMoveComp->IsZooming();
		if (bNow != bPrevZoomState)
		{
			bPrevZoomState = bNow;
			CachedCrosshair->SetZoom(bNow);
		}
	}
}

void ASTStagePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (ASTPlayerCharacter* PlayerCharacter = Cast<ASTPlayerCharacter>(InPawn))
	{
		if (PlayerCharacter->GetHealthComponent())
		{
			PlayerCharacter->GetHealthComponent()->OnHealthChanged.AddDynamic(
				this, &ASTStagePlayerController::UpdateHealth);

			PlayerCharacter->GetHealthComponent()->OnHealthHeal.AddDynamic(
				this, &ASTStagePlayerController::HandlePlayerHealed);
			
			PlayerCharacter->GetHealthComponent()->OnHealthDamage.AddDynamic(
			    this, &ASTStagePlayerController::HandlePlayerDamaged);
		}

		

		if (USTWeaponManagerComponent* WeaponManager = PlayerCharacter->FindComponentByClass<USTWeaponManagerComponent>())
		{
			WeaponManager->EquipDelegate.AddDynamic(this, &ASTStagePlayerController::UpdateWeapon);
			WeaponManager->AmmoChangeDelegate.AddDynamic(this, &ASTStagePlayerController::UpdateAmmo);
		}
		
		CachedMoveComp = PlayerCharacter->GetPlayerMovementComponent();
	}
}

void ASTStagePlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	if (ASTPlayerCharacter* PlayerCharacter = Cast<ASTPlayerCharacter>(GetPawn()))
	{
		if (USTHealthComponent* HC = PlayerCharacter->GetHealthComponent())
		{
			HC->OnHealthChanged.RemoveDynamic(this, &ASTStagePlayerController::UpdateHealth);
			HC->OnHealthHeal.RemoveDynamic(this, &ASTStagePlayerController::HandlePlayerHealed);
			HC->OnHealthDamage.RemoveDynamic(this, &ASTStagePlayerController::HandlePlayerDamaged);
		}
	}

	// 엔드 화면이 아니더라도, 폰이 사라진 시점엔 입력/시점 모두 차단
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);
	DisableInput(this);

	SetInputMode(FInputModeUIOnly());
	bShowMouseCursor = true;
}

// 히트마커&데미지량
static bool IsHeadShot(const FName& BoneName)
{
	const FString B = BoneName.ToString();
	return B.Equals(TEXT("head"), ESearchCase::IgnoreCase)
		|| B.Equals(TEXT("Head"), ESearchCase::IgnoreCase)
		|| B.Contains(TEXT("head"), ESearchCase::IgnoreCase);
}

// 진행 상황 텍스트
void ASTStagePlayerController::RefreshMissionProgress(int32 ProgressIndex)
{
	if (!ScoreboardWidget) return;

	if (ASTGameState* STGameState = Cast<ASTGameState>(GetWorld()->GetGameState()))
	{
		const auto& Info = STGameState->GetGameStateInfo();
		const TArray<FText>& List = Info.StageProgressList;

		if (List.IsValidIndex(ProgressIndex))
		{
			ScoreboardWidget->UpdateMissionProgress(List[ProgressIndex].ToString());
		}
	}
}

// 플레이어 회복 로그
void ASTStagePlayerController::HandlePlayerHealed(float HealAmount)
{
	const int32 ShownLog = FMath::Max(1, FMath::RoundToInt(HealAmount));
	AddDamageKillLog(FString::Printf(TEXT("플레이어가 %d 회복했습니다."), ShownLog));
}

// 플레이어 피해 로그
void ASTStagePlayerController::HandlePlayerDamaged(float DamageAmount)
{
	const int32 ShownLog = FMath::Max(1, FMath::RoundToInt(DamageAmount));
	AddDamageKillLog(FString::Printf(TEXT("플레이어가 %d 피해를 받았습니다."), ShownLog));
}

void ASTStagePlayerController::HandleEnemyDamageTaken(AActor* DamagedActor, float DamageAmount, bool bCritical)
{
	ShowHitMarker();

	// 에너미 피해 로그 출력(피격지 데미지 UI가 있어서 보류)
	/*
	const int32 ShownLog = FMath::Max(1, FMath::RoundToInt(DamageAmount));
	AddDamageKillLog(FString::Printf(TEXT("적에게 %d 피해를 주었습니다."), ShownLog));
	*/
	
	FVector WorldLoc = DamagedActor ? DamagedActor->GetActorLocation() : FVector::ZeroVector;

	if (const ACharacter* Char = Cast<ACharacter>(DamagedActor))
	{
		if (USkeletalMeshComponent* Mesh = Char->GetMesh())
		{
			static const FName HeadSocket(TEXT("head"));
			if (Mesh->DoesSocketExist(HeadSocket))
			{
				WorldLoc = Mesh->GetSocketLocation(HeadSocket) + FVector(0,0,12.f);
			}
			else if (const UCapsuleComponent* Capsule = Char->GetCapsuleComponent())
			{
				WorldLoc = Char->GetActorLocation() + FVector(0,0, Capsule->GetScaledCapsuleHalfHeight() + 10.f);
			}
		}
	}
	else
	{
		WorldLoc += FVector(0,0,100.f);
	}

	WorldLoc = GetShotgunScreenRandomLoc(WorldLoc);
	
	const int32 Shown = FMath::Max(1, FMath::RoundToInt(DamageAmount));
	if (StageWidget)
	{
		StageWidget->ShowDamageTextAtEx(WorldLoc, Shown, bCritical);
	}
}

FVector ASTStagePlayerController::GetShotgunScreenRandomLoc(const FVector& BaseWorldLoc) const
{
	if (!bIsShotgunWeapon || !PlayerCameraManager)
	{
		return BaseWorldLoc;
	}

	// 화면 크기
	int32 SizeX = 0, SizeY = 0;
	this->GetViewportSize(SizeX, SizeY);
	if (SizeX <= 0 || SizeY <= 0)
	{
		return BaseWorldLoc;
	}

	// 카메라→표시기준점 거리
	const FVector CamLoc = PlayerCameraManager->GetCameraLocation();
	const float   Depth  = (BaseWorldLoc - CamLoc).Size();

	// 화면 중앙 좌표
	const float CX = SizeX * 0.5f;
	const float CY = SizeY * 0.5f;

	// 중앙 기준 원판 내 랜덤 오프셋(px)
	const float r     = DamageTextScreenRadiusPx; // 예: 60.f
	const float angle = FMath::FRandRange(0.f, 2.f * PI);
	const float rad   = FMath::FRandRange(0.f, r);
	const float dx    = FMath::Cos(angle) * rad;
	const float dy    = FMath::Sin(angle) * rad;

	// 스크린 → 월드 역투영
	FVector Origin, Dir;
	if (this->DeprojectScreenPositionToWorld(CX + dx, CY + dy, Origin, Dir))
	{
		return Origin + Dir * Depth;
	}

	return BaseWorldLoc;
}

void ASTStagePlayerController::ShowDamageNumberAtActor(AActor* Target, int32 Damage, bool bCritical, FName SocketName)
{
	if (!Target || !StageWidget) return;

	FVector WorldLoc = Target->GetActorLocation();
	
	if (const ACharacter* Char = Cast<ACharacter>(Target))
	{
		if (USkeletalMeshComponent* Mesh = Char->GetMesh())
		{
			static const FName HeadSocket(TEXT("head"));

			if (Mesh->DoesSocketExist(HeadSocket))
			{
				WorldLoc = Mesh->GetSocketLocation(HeadSocket) + FVector(0, 0, 12.f);
			}
			else
			{
				if (const UCapsuleComponent* Capsule = Char->GetCapsuleComponent())
				{
					WorldLoc = Char->GetActorLocation() + FVector(0, 0, Capsule->GetScaledCapsuleHalfHeight() + 10.f);
				}
			}
		}
	}
	else
	{
		WorldLoc += FVector(0, 0, 100.f);
	}
	
	WorldLoc = GetShotgunScreenRandomLoc(WorldLoc);
	
	StageWidget->ShowDamageTextAtEx(WorldLoc, Damage, bCritical);
}

// 킬 확정 표시
void ASTStagePlayerController::HandleEnemyDied_ShowConfirm(AActor* DeadEnemy)
{
	ShowKillConfirmed();

	// 킬 로그 출력
	AddDamageKillLog(TEXT("적을 처치하였습니다."));

	// 처치 수 증가 후 UI 반영
	++KilledEnemyCount;
	UpdateEnemyStatus(KilledEnemyCount, TotalEnemyCount);
}

// 점수판 표시
void ASTStagePlayerController::ShowScoreboard()
{
	// 실제 데이터 대신 임시 값으로 전달
	int32 TempScore = 1230;                  // 임시 점수
	int32 TempKills = 7;                     // 임시 킬 수
	FString TempGoal = TEXT("적 섬멸 후 목표지점 이동");   // 목표 텍스트
	FString TempProgress = TEXT("적을 섬멸하세요");       // 진행 텍스트

	// PlayerState에서 점수/킬 최신값
	if (ASTPlayerState* PS = GetPlayerState<ASTPlayerState>())
	{
		const FPlayerStateInfo& Info = PS->GetPlayerStateInfo();
		TempScore = Info.Score;
		TempKills = Info.KillCount;
	}

	// GameState에서 목표/진행 문구
	if (ASTGameState* STGameState = Cast<ASTGameState>(GetWorld()->GetGameState()))
	{
		const auto& GSInfo = STGameState->GetGameStateInfo();

		// 목표 텍스트
		TempGoal = GSInfo.StageGoalText.ToString();

		// 진행 텍스트
		const TArray<FText>& ProgressList = GSInfo.StageProgressList;                  
		const bool bAllKilled = (TotalEnemyCount > 0 && KilledEnemyCount >= TotalEnemyCount);
		const int32 ProgressIdx = bAllKilled ? 1 : 0;                                  
		if (ProgressList.IsValidIndex(ProgressIdx))                                    
		{
			TempProgress = ProgressList[ProgressIdx].ToString();                       
		}
	}


	if (ScoreboardWidget)
	{
		// 점수/킬 갱신
		ScoreboardWidget->UpdateScoreAndKill(TempScore, TempKills);

		// 목표/진행 텍스트 갱신
		if (!TempGoal.IsEmpty())
		{
			ScoreboardWidget->UpdateMissionGoal(TempGoal);
		}
		if (!TempProgress.IsEmpty())
		{
			ScoreboardWidget->UpdateMissionProgress(TempProgress);
		}

		ScoreboardWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

// 점수판 숨김
void ASTStagePlayerController::HideScoreboard()
{
	if (ScoreboardWidget)
	{
		ScoreboardWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ASTStagePlayerController::TogglePauseMenu()
{
	if (IsPaused())
	{
		// 게임 재개
		SetPause(false);

		if (PauseMenuWidget)
		{
			PauseMenuWidget->SetVisibility(ESlateVisibility::Hidden);
		}

		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
	}
	else
	{
		// 게임 일시정지
		SetPause(true);

		if (!PauseMenuWidget && PauseMenuWidgetClass)
		{
			PauseMenuWidget = CreateWidget<USTPauseMenuWidget>(this, PauseMenuWidgetClass);
			if (PauseMenuWidget)
			{
				PauseMenuWidget->AddToViewport(100);
				PauseMenuWidget->OnReturnToMainRequested.AddDynamic(this, &ASTStagePlayerController::HandlePauseReturnToMain);
				PauseMenuWidget->OnQuitGameRequested.AddDynamic(this, &ASTStagePlayerController::HandleQuitGame);
			}
		}

		if (PauseMenuWidget)
		{
			PauseMenuWidget->SetVisibility(ESlateVisibility::Visible);
		}

		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
	}
}

void ASTStagePlayerController::HandlePauseReturnToMain()
{
	// JM : 수정 / 레벨이동 로직 한 군데에서 관리
	SetPause(false);
	SetInputMode(FInputModeUIOnly());
	bShowMouseCursor = true;
	LoadLevelWithDataResetAndLoadingScreen(EStageType::MainMenu);
	
	/*if (USTGameInstance* GI = GetGameInstance<USTGameInstance>())
	{
		SetPause(false);
		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
		
		GI->GoToMainMenu();
	}*/
}

void ASTStagePlayerController::HandleQuitGame()
{
	if (USTGameInstance* STGameInstance = GetGameInstance<USTGameInstance>())
	{
		STGameInstance->QuitGame();
	}
}

void ASTStagePlayerController::TriggerGameOverWithTempData()
{
	// 임시값
	int32 TempScore       = 12450;
	int32 TempKillCount   = 9;
	int32 TempDamageDealt = 30500;
	int32 TempDamageTaken = 11200;

	// PlayerState 값이 있으면 덮어쓰기
	if (ASTPlayerState* PS = GetPlayerState<ASTPlayerState>())
	{
		const FPlayerStateInfo& Info = PS->GetPlayerStateInfo();

		// 필요 시 가져와서 임시값 대체
		TempScore       = Info.Score;
		TempKillCount   = Info.KillCount;
		TempDamageDealt = Info.TotalDamageInflicted;
		TempDamageTaken = Info.TotalDamageReceived;
	}
	
	ShowGameOverResult(TempScore, TempKillCount, TempDamageDealt, TempDamageTaken, PendingGameOverReason);
}

void ASTStagePlayerController::TriggerGameClearWithTempData()
{
	if (bGameClearShown)
	{
		return;
	}
	bGameClearShown = true;
	
	// 기본 임시값
	int32 TempScore     = 15000;
	int32 TempHighScore = 20000;

	// PlayerState 있으면 일부 값 대체
	if (ASTPlayerState* PS = GetPlayerState<ASTPlayerState>())
	{
		const FPlayerStateInfo& Info = PS->GetPlayerStateInfo();
		TempScore     = Info.Score;
		TempHighScore = Info.HighScore;
	}
	
	ShowGameClearResult();
}

void ASTStagePlayerController::ScheduleGameOver(float DelaySeconds)
{
	if (bGameOverShown)
	{
		return;
	}

	// 중복 타이머 방지
	if (GetWorldTimerManager().IsTimerActive(GameOverTimerHandle))
	{
		return;
	}
	
	bGameOverShown = true;

	// 카메라 페이드
	if (PlayerCameraManager)
	{
		PlayerCameraManager->StartCameraFade(
			0.f,
			1.f,
			FMath::Max(0.f, DelaySeconds),
			FLinearColor::Black,
			false,
			true
		);
	}

	GetWorldTimerManager().SetTimer(
		GameOverTimerHandle,
		this,
		&ASTStagePlayerController::TriggerGameOverWithTempData,
		FMath::Max(0.f, DelaySeconds),
		false
	);
}


void ASTStagePlayerController::UpdateHealth(float CurrentHP, float MaxHP)
{
	if (StageWidget)
	{
		StageWidget->UpdateHealth(CurrentHP, MaxHP);
	}

	if (CurrentHP <= 0.f)
	{
		PendingGameOverReason = NSLOCTEXT("GameOver", "PlayerDead", "캐릭터가 사망했습니다.");
		
		SetIgnoreMoveInput(true);
		SetIgnoreLookInput(true);
		DisableInput(this);
		
		if (ACharacter* C = Cast<ACharacter>(GetPawn()))
		{
			if (UCharacterMovementComponent* Move = C->GetCharacterMovement())
			{
				Move->StopMovementImmediately();
				Move->DisableMovement();
			}
			if (UCapsuleComponent* Capsule = C->GetCapsuleComponent())
			{
				Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
		
		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
	}
}

void ASTStagePlayerController::UpdateWeapon(const FString& WeaponName)
{
	CurrentWeaponName = WeaponName;

	bIsShotgunWeapon = WeaponName.Equals(TEXT("Shotgun"), ESearchCase::IgnoreCase);
	
	if (StageWidget)
	{
		StageWidget->UpdateWeapon(WeaponName);
	}
}

void ASTStagePlayerController::UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (StageWidget)
		StageWidget->UpdateAmmo(CurrentAmmo, MaxAmmo);
}

void ASTStagePlayerController::UpdateTimer(int32 RemainingSeconds)
{
	// UE_LOG(LogSystem, Log, TEXT("ASTStagePlayerController::UpdateTimer(%d) Start"), RemainingSeconds);    // JM

	if (StageWidget)
	{
		StageWidget->UpdateTimer(RemainingSeconds);
	}

	if (RemainingSeconds <= 0)
	{
		PendingGameOverReason = NSLOCTEXT("GameOver", "TimeOver", "제한 시간을 초과하셨습니다.");
	}
	
	// UE_LOG(LogSystem, Log, TEXT("ASTStagePlayerController::UpdateTimer(%d) End"), RemainingSeconds);    // JM
}

void ASTStagePlayerController::UpdateEnemyStatus(int32 Killed, int32 Total)
{
	if (StageWidget)
		StageWidget->UpdateEnemyStatus(Killed, Total);
	

	// 모두 처치되면 스코어보드 진행 텍스트를 [1]로 전환
	if (Killed == Total)
	{
		RefreshMissionProgress(1);
	}
}

void ASTStagePlayerController::AddDamageKillLog(const FString& LogText)
{
	if (StageWidget)
		StageWidget->AddDamageKillLog(LogText);
}

void ASTStagePlayerController::ShowHitMarker()
{
	if (StageWidget)
		StageWidget->ShowHitMarker();
}

void ASTStagePlayerController::ShowKillConfirmed()
{
	if (StageWidget)
		StageWidget->ShowKillConfirmedImage();
}

void ASTStagePlayerController::ShowDamageTextAt(FVector WorldLocation, int32 Damage)
{
	if (StageWidget)
		StageWidget->ShowDamageTextAt(WorldLocation, Damage);
}

void ASTStagePlayerController::ShowGameOverResult(int32 Score, int32 KillCount, int32 DamageDealt, int32 DamageTaken, const FText& ReasonText)
{
	if (!GameOverWidget && GameOverWidgetClass)
	{
		GameOverWidget = CreateWidget<USTGameOverWidget>(this, GameOverWidgetClass);
		if (GameOverWidget)
		{
			GameOverWidget->AddToViewport(500);

			if (PlayerCameraManager)
			{
				PlayerCameraManager->StartCameraFade(1.f, 0.f, 0.25f, FLinearColor::Black, false, false);
			}

			GameOverWidget->OnRetryRequested.AddDynamic(this, &ASTStagePlayerController::HandleGameOverRetry);
			GameOverWidget->OnReturnToMainRequested.AddDynamic(this, &ASTStagePlayerController::HandleGameOverReturnToMain);
		}
	}

	if (GameOverWidget)
	{
		GameOverWidget->SetResultInfo(Score, KillCount, DamageDealt, DamageTaken, ReasonText);
		SetPause(true);
		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
	}
}

void ASTStagePlayerController::ShowGameClearResult()
{
	UE_LOG(LogSystem, Log, TEXT("ASTStagePlayerController::ShowGameClearResult() Start"));
	if (!GameClearWidget && GameClearWidgetClass)
	{
		GameClearWidget = CreateWidget<USTGameClearWidget>(this, GameClearWidgetClass);
		if (GameClearWidget)
		{
			GameClearWidget->AddToViewport(600); // ZOrder 높게 설정
			
			GameClearWidget->OnRetryRequested.AddDynamic(this, &ASTStagePlayerController::HandleGameClearRetry);
			GameClearWidget->OnReturnToMainRequested.AddDynamic(this, &ASTStagePlayerController::HandleGameClearReturnToMain);
		}
	}

	int32 Score = 0, HighScore = 0;
	if (const ASTPlayerState* STPlayerState = GetPlayerState<ASTPlayerState>())
	{
		const FPlayerStateInfo& PlayerStateInfo = STPlayerState->GetPlayerStateInfo();
		Score = PlayerStateInfo.Score;
		HighScore = PlayerStateInfo.HighScore;
	}
	else
	{
		UE_LOG(LogSystem, Warning, TEXT("ASTStagePlayerController::ShowGameClearResult() Can't Get STPlayerState"));
	}

	if (GameClearWidget)
	{
		GameClearWidget->SetResultInfo(Score, HighScore);
		SetPause(true);
		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
	}
	UE_LOG(LogSystem, Log, TEXT("ASTStagePlayerController::ShowGameClearResult() End"));
}

// JM: 레벨 블루프린트에서 재생 중인 BGM 정지
void ASTStagePlayerController::StopLevelBGM()
{
	UE_LOG(LogSystem, Log, TEXT("ASTStagePlayerController::StopLevelBGM() Start"));

	ALevelScriptActor* LevelBPActor = GetWorld()->GetLevelScriptActor();
	if (!LevelBPActor)
	{
		UE_LOG(LogSystem, Warning, TEXT("ASTStagePlayerController::StopLevelBGM() LevelBPActor Not Found"));
		return;
	}
	
	TArray<UAudioComponent*> AudioComponents;
	LevelBPActor->GetComponents<UAudioComponent>(AudioComponents);
	
	if (AudioComponents.Num() > 0)
	{
		UAudioComponent* BGMComp = AudioComponents[0];
		if (BGMComp->IsPlaying())
		{
			BGMComp->Stop();
			UE_LOG(LogSystem, Log, TEXT("ASTStagePlayerController::StopLevelBGM() Stop Level BGM"));
		}
		else
		{
			UE_LOG(LogSystem, Warning, TEXT("ASTStagePlayerController::StopLevelBGM() BGMComponent is not playing"));
		}
	}
	else
	{
		UE_LOG(LogSystem, Warning, TEXT("ASTStagePlayerController::StopLevelBGM() BGMComponent not found"));
	}
	
	UE_LOG(LogSystem, Log, TEXT("ASTStagePlayerController::StopLevelBGM() End"));
}


// JM: 스테이지 클리어시 호출됨
void ASTStagePlayerController::HandleStageClear()
{
	UE_LOG(LogSystem, Log, TEXT("ASTStagePlayerController::HandleStageClear() Start"));
	
	// 현재 스테이지 정보 가져오기
	USTGameInstance* STGameInstance = GetGameInstance<USTGameInstance>();
	if (!STGameInstance) return;

	// FPlayerStateInfo -> GameInstance에 저장
	if(ASTPlayerState* STPlayerState = GetPlayerState<ASTPlayerState>())
	{
		STGameInstance->PlayerStateInfo = STPlayerState->GetPlayerStateInfo();
	}
	
	EStageType NextStage = EStageType::None;
	int32 LoadingScreenIndex = 0;
	switch (STGameInstance->LastStage)
	{
		case EStageType::Stage1:
			NextStage = EStageType::Stage2;
			LoadingScreenIndex = 2;
			break;
		case EStageType::Stage2:
			NextStage = EStageType::Stage3;
			LoadingScreenIndex = 3;
			break;
		case EStageType::Stage3:
			NextStage = EStageType::Ending;
			LoadingScreenIndex = 4;		// TODO: 보여줄 로딩화면 처리 (아직 없긴함) - 엔딩시네마틱?
			break;
		default:	
			break;
	}

	if (NextStage == EStageType::Stage2 || NextStage == EStageType::Stage3)
	{
		/*STGameInstance->LastStage = NextStage*/;
		LoadNextStage_BP(NextStage, LoadingScreenIndex);	// 다음 스테이지로 이동(BP Implement 이벤트) / 로딩화면 조절가능
	}
	else if (NextStage == EStageType::Ending)
	{
		ShowGameClearResult();		// JM : 게임 클리어 UI 띄우기
		StopLevelBGM();				// JM : 기존 레벨BP BGM 정지
		PlayGameClearBGM_BP();		// JM : 게임 클리어시 BGM 재생
	}
	else
	{
		UE_LOG(LogSystem, Warning, TEXT("ASTStagePlayerController::HandleStageClear() NextStage == NONE!"));	
	}
	
	UE_LOG(LogSystem, Log, TEXT("ASTStagePlayerController::HandleStageClear() End"));
}

void ASTStagePlayerController::HandleStageFailed()
{
	UE_LOG(LogSystem, Log, TEXT("ASTStagePlayerController::HandleStageFailed() Start"));

	// 1) 플레이어 입력/이동 즉시 차단
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);
	DisableInput(this);

	// 2) 캐릭터 즉시 멈추기 & 캡슐 충돌 끄기
	if (ACharacter* C = Cast<ACharacter>(GetPawn()))
	{
		if (UCharacterMovementComponent* Move = C->GetCharacterMovement())
		{
			Move->StopMovementImmediately();
			Move->DisableMovement();
		}
		if (UCapsuleComponent* Capsule = C->GetCapsuleComponent())
		{
			Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	// 3) UI 전용 입력 모드 + 마우스 커서 ON
	SetInputMode(FInputModeUIOnly());
	bShowMouseCursor = true;

	// 4) 페이드 후 GameOver UI 띄우기
	ScheduleGameOver(GameOverDelay);
	
	UE_LOG(LogSystem, Log, TEXT("ASTStagePlayerController::HandleStageFailed() End"));
}

//Game Over 버튼
void ASTStagePlayerController::HandleGameOverRetry()
{
	LoadLevelWithDataResetAndLoadingScreen(EStageType::Stage1);
	/*if (USTGameInstance* GI = GetGameInstance<USTGameInstance>())
	{
		// GI->GoToLevel(EStageType::Stage1);
		// GI->GoToRetry();
		
		// JM 수정 / 로딩화면 컨트롤하기 위함 (Stage1 로딩화면 재생)
		GI->ResetDataForRetry();		// 데이터 초기화
		LoadNextStage_BP(EStageType::Stage1, 1);
	}*/
}

void ASTStagePlayerController::HandleGameOverReturnToMain()
{
	LoadLevelWithDataResetAndLoadingScreen(EStageType::MainMenu);
	/*if (USTGameInstance* GI = GetGameInstance<USTGameInstance>())
	{
		GI->GoToMainMenu();
	}*/
}

//Game Clear 버튼
void ASTStagePlayerController::HandleGameClearRetry()
{
	LoadLevelWithDataResetAndLoadingScreen(EStageType::Stage1);
	/*if (USTGameInstance* GI = GetGameInstance<USTGameInstance>())
	{
		// GI->GoToLevel(EStageType::Stage1);
		GI->GoToRetry();	// JM : 초기화 되는 Retry는 이쪽으로
	}*/
}

void ASTStagePlayerController::HandleGameClearReturnToMain()
{
	LoadLevelWithDataResetAndLoadingScreen(EStageType::MainMenu);
	
	/*
	if (USTGameInstance* GI = GetGameInstance<USTGameInstance>())
	{
		GI->GoToMainMenu();
	}*/
}

// JM: 레벨이동 통합관리
void ASTStagePlayerController::LoadLevelWithDataResetAndLoadingScreen(const EStageType& NextStage)
{
	UE_LOG(LogSystem, Warning, TEXT("ASTStagePlayerController::LoadLevelWithDataResetAndLoadingScreen(%s) Start"), *StaticEnum<EStageType>()->GetValueAsString(NextStage));
	if (USTGameInstance* GI = GetGameInstance<USTGameInstance>())
	{
		GI->ResetDataForRetry();
		int LoadingScreenIndex = 0;
		switch (NextStage)
		{
			case EStageType::MainMenu:
				LoadingScreenIndex = 0;
				break;
			case EStageType::Stage1:
				LoadingScreenIndex = 1;
				break;
			default:
				UE_LOG(LogSystem, Warning, TEXT("ASTStagePlayerController::LoadLevelWithDataResetAndLoadingScreen(%s) Not this case Implemented"), *StaticEnum<EStageType>()->GetValueAsString(NextStage));
		}
		LoadNextStage_BP(NextStage, LoadingScreenIndex);
	}
	UE_LOG(LogSystem, Warning, TEXT("ASTStagePlayerController::LoadLevelWithDataResetAndLoadingScreen(%s) End"), *StaticEnum<EStageType>()->GetValueAsString(NextStage));
}