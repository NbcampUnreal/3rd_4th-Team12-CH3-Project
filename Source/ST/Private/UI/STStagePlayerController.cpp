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
#include "GameFramework/CharacterMovementComponent.h"

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
	UpdateWeapon(TEXT("라이플")); // 임시 무기 이름
	UpdateAmmo(25, 90); //	탄약: 현재 25발 / 최대 90발
	UpdateEnemyStatus(0, 10); // 적 처치: 0 / 총 10명
	AddDamageKillLog(TEXT("10의 피해를 받았습니다.")); // 로그 메시지

	//게임 오버 화면 테스트
	//ShowGameOverResult(12450, 22, 30000, 10500);

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
	}
	
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
		}
		
		CachedMoveComp = PlayerCharacter->GetPlayerMovementComponent();
	}
}

void ASTStagePlayerController::OnUnPossess()
{
	Super::OnUnPossess();

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

void ASTStagePlayerController::HandleEnemyDamageTaken(AActor* DamagedActor, float DamageAmount, bool bCritical)
{
	ShowHitMarker();
	
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
	
	const int32 Shown = FMath::Max(1, FMath::RoundToInt(DamageAmount));
	if (StageWidget)
	{
		StageWidget->ShowDamageTextAtEx(WorldLoc, Shown, bCritical);
	}
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
	
	StageWidget->ShowDamageTextAtEx(WorldLoc, Damage, bCritical);
}

// 킬 확정 표시
void ASTStagePlayerController::HandleEnemyDied_ShowConfirm(AActor* DeadEnemy)
{
	ShowKillConfirmed();
}

// 점수판 표시
void ASTStagePlayerController::ShowScoreboard()
{
	// 실제 데이터 대신 임시 값으로 전달
	int32 TempScore = 1230;                  // 임시 점수
	int32 TempKills = 7;                     // 임시 킬 수
	FString TempGoal = TEXT("적 섬멸 후 목표지점 이동");   // 목표 텍스트
	FString TempProgress = TEXT("적을 섬멸하세요");       // 진행 텍스트

	ScoreboardWidget->UpdateScoreAndKill(TempScore, TempKills);
	ScoreboardWidget->UpdateMissionGoal(TempGoal);
	ScoreboardWidget->UpdateMissionProgress(TempProgress);
	
	if (ScoreboardWidget)
	{
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
	if (USTGameInstance* GI = GetGameInstance<USTGameInstance>())
	{
		SetPause(false);
		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
		
		GI->GoToMainMenu();
	}
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
	if (bGameOverShown)
	{
		return;
	}
	bGameOverShown = true;

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
	}
	
	ShowGameOverResult(TempScore, TempKillCount, TempDamageDealt, TempDamageTaken);
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
	
	ShowGameClearResult(TempScore, TempHighScore);
}




void ASTStagePlayerController::UpdateHealth(float CurrentHP, float MaxHP)
{
	if (StageWidget)
	{
		StageWidget->UpdateHealth(CurrentHP, MaxHP);
	}

	if (CurrentHP <= 0.f)
	{
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
		
		TriggerGameOverWithTempData();
	}
}

void ASTStagePlayerController::UpdateWeapon(const FString& WeaponName)
{
	if (StageWidget)
		StageWidget->UpdateWeapon(WeaponName);
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
		TriggerGameOverWithTempData();
	}

	// UE_LOG(LogSystem, Log, TEXT("ASTStagePlayerController::UpdateTimer(%d) End"), RemainingSeconds);    // JM
}

void ASTStagePlayerController::UpdateEnemyStatus(int32 Killed, int32 Total)
{
	if (StageWidget)
		StageWidget->UpdateEnemyStatus(Killed, Total);
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

void ASTStagePlayerController::ShowGameOverResult(int32 Score, int32 KillCount, int32 DamageDealt, int32 DamageTaken)
{
	if (!GameOverWidget && GameOverWidgetClass)
	{
		GameOverWidget = CreateWidget<USTGameOverWidget>(this, GameOverWidgetClass);
		if (GameOverWidget)
		{
			GameOverWidget->AddToViewport(500);

			GameOverWidget->OnRetryRequested.AddDynamic(this, &ASTStagePlayerController::HandleGameOverRetry);
			GameOverWidget->OnReturnToMainRequested.AddDynamic(this, &ASTStagePlayerController::HandleGameOverReturnToMain);
		}
	}

	if (GameOverWidget)
	{
		GameOverWidget->SetResultInfo(Score, KillCount, DamageDealt, DamageTaken);
		SetPause(true);
		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
	}
}

void ASTStagePlayerController::ShowGameClearResult(int32 Score, int32 HighScore)
{
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

	if (GameClearWidget)
	{
		GameClearWidget->SetResultInfo(Score, HighScore);
		SetPause(true);
		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
	}
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
		// TODO: 보스스테이지 클리어시 처리
		default:	
			break;
	}

	if (NextStage != EStageType::None)
	{
		STGameInstance->LastStage = NextStage;
		LoadNextStage_BP(NextStage, LoadingScreenIndex);	// 다음 스테이지로 이동(BP Implement 이벤트)
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

	// TODO: 스테이지 실패 화면 띄우기
	
	UE_LOG(LogSystem, Log, TEXT("ASTStagePlayerController::HandleStageFailed() End"));
}

//Game Over 버튼
void ASTStagePlayerController::HandleGameOverRetry()
{
	if (USTGameInstance* GI = GetGameInstance<USTGameInstance>())
	{
		GI->GoToLevel(EStageType::Stage1);
	}
}

void ASTStagePlayerController::HandleGameOverReturnToMain()
{
	if (USTGameInstance* GI = GetGameInstance<USTGameInstance>())
	{
		GI->GoToMainMenu();
	}
}

//Game Clear 버튼
void ASTStagePlayerController::HandleGameClearRetry()
{
	if (USTGameInstance* GI = GetGameInstance<USTGameInstance>())
	{
		GI->GoToLevel(EStageType::Stage1);
	}
}

void ASTStagePlayerController::HandleGameClearReturnToMain()
{
	if (USTGameInstance* GI = GetGameInstance<USTGameInstance>())
	{
		GI->GoToMainMenu();
	}
}