#include "UI/STStagePlayerController.h"
#include "UI/STStageWidget.h"
#include "UI/STPauseMenuWidget.h"
#include "UI/STScoreboardWidget.h"
#include "UI/STGameOverWidget.h"
#include "UI/STGameClearWidget.h"
#include "Blueprint/UserWidget.h"
#include "System/STGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/STHealthComponent.h"
#include "Player/STPlayerCharacter.h"
#include "System/STGameMode.h"
#include "System/STGameState.h"
#include "System/STLog.h"
#include "System/STPlayerState.h"

ASTStagePlayerController::ASTStagePlayerController()
{
	bShowMouseCursor = false;

	
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


	ASTPlayerCharacter* PlayerCharacter = Cast<ASTPlayerCharacter>(GetPawn()); 
	if (PlayerCharacter)
	{
		PlayerCharacter->GetHealthComponent()->OnHealthChanged.AddDynamic(this,&ASTStagePlayerController::UpdateHealth);
	}
	
	// 실제 데이터 대신 임시 값으로 전달
	UpdateWeapon(TEXT("라이플")); // 임시 무기 이름
	UpdateAmmo(25, 90); // 탄약: 현재 25발 / 최대 90발
	UpdateTimer(180); // 제한시간: 180초 남음
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

void ASTStagePlayerController::UpdateHealth(float CurrentHP, float MaxHP)
{
	if (StageWidget)
		StageWidget->UpdateHealth(CurrentHP, MaxHP);
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
	if (StageWidget)
		StageWidget->UpdateTimer(RemainingSeconds);
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
		}
	}

	if (GameOverWidget)
	{
		GameOverWidget->SetResultInfo(Score, KillCount, DamageDealt, DamageTaken);
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
		}
	}

	if (GameClearWidget)
	{
		GameClearWidget->SetResultInfo(Score, HighScore);
		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
	}
}

void ASTStagePlayerController::HandleQuitGame()
{
	//임시코드(여기에서 종료)
	UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
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