#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "System/STGameTypes.h"
#include "STStagePlayerController.generated.h"

class UAudioComponent;
class USoundBase;

class UUserWidget;
class USTStageWidget;
class USTPauseMenuWidget;
class USTScoreboardWidget;
class USTGameOverWidget;
class USTGameClearWidget;
class UUWCrosshairWidget;

class USTMovementComponent;
class USTBossBarWidget;

class ASTEnemyBoss;
class ASTEnemyBossAIController;

UCLASS()
class ST_API ASTStagePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// ===== 생성자 & 기본 오버라이드 =====
	ASTStagePlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	// ===== UI 업데이트 =====
	UFUNCTION() void UpdateHealth(float CurrentHP, float MaxHP);
	UFUNCTION() void UpdateWeapon(const FString& WeaponName);
	UFUNCTION() void UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo);
	UFUNCTION(BlueprintCallable, Category="Timer") void UpdateTimer(int32 RemainingSeconds);
	UFUNCTION() void UpdateEnemyStatus(int32 Killed, int32 Total);
	UFUNCTION() void AddDamageKillLog(const FString& LogText);

	// ===== 전투 피드백 =====
	UFUNCTION() void ShowHitMarker();
	UFUNCTION() void ShowKillConfirmed();
	UFUNCTION() void ShowDamageTextAt(FVector WorldLocation, int32 Damage);
	UFUNCTION() void HandleEnemyDied_ShowConfirm(AActor* DeadEnemy);

	// ===== 메뉴 / 보스 UI =====
	void TogglePauseMenu();

	UFUNCTION(BlueprintCallable, Category="UI|Boss") void ShowBossBar(AActor* BossActor);
	UFUNCTION(BlueprintCallable, Category="UI|Boss") void HideBossBar();

	UFUNCTION() void HandleBossRecognizedPlayer();

	// 보스 AI
	UPROPERTY() ASTEnemyBossAIController* CachedBossAI = nullptr;
	bool bBossUIActivated = false;

	// ===== 메뉴/스테이지 전환 핸들러 =====
	UFUNCTION() void HandlePauseReturnToMain();
	UFUNCTION() void HandleQuitGame();
	UFUNCTION() void HandleStageClear();
	UFUNCTION() void HandleStageFailed();

	// ===== GameOver / GameClear 버튼 =====
	UFUNCTION() void HandleGameOverRetry();
	UFUNCTION() void HandleGameOverReturnToMain();
	UFUNCTION() void HandleGameClearRetry();
	UFUNCTION() void HandleGameClearReturnToMain();
	UFUNCTION() void HandlePlayEndingRequested();

	// ===== 결과 UI 표시 =====
	UFUNCTION() void ShowGameOverResult(int32 Score, int32 KillCount, int32 DamageDealt, int32 DamageTaken, const FText& ReasonText);
	UFUNCTION() void ShowGameClearResult();

	// ===== BGM =====
	void PlayAnotherBGM(const EBGMType& BGMType);
	UFUNCTION() void PlayBossBGM(int32 OldPhaseInt, int32 NewPhaseInt);

	// ===== 로딩 화면 (BP 이벤트) =====
	UFUNCTION(BlueprintImplementableEvent)
	void LoadNextStage_BP(EStageType NextStage, int32 LoadingScreenIndex);

protected:
	// ===== 위젯 클래스 / 인스턴스 =====
	UPROPERTY(EditDefaultsOnly, Category="UI") TSubclassOf<UUserWidget> StageWidgetClass;
	UPROPERTY() USTStageWidget* StageWidget = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="UI") TSubclassOf<UUserWidget> PauseMenuWidgetClass;
	UPROPERTY() USTPauseMenuWidget* PauseMenuWidget = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="UI") TSubclassOf<USTScoreboardWidget> ScoreboardWidgetClass;
	UPROPERTY() USTScoreboardWidget* ScoreboardWidget = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="UI") TSubclassOf<USTGameOverWidget> GameOverWidgetClass;
	UPROPERTY() USTGameOverWidget* GameOverWidget = nullptr;

	UPROPERTY(EditDefaultsOnly, Category="UI") TSubclassOf<USTGameClearWidget> GameClearWidgetClass;
	UPROPERTY() USTGameClearWidget* GameClearWidget = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI|Boss", meta=(AllowPrivateAccess="true"))
	TSubclassOf<USTBossBarWidget> BossBarWidgetClass;
	UPROPERTY() USTBossBarWidget* BossBarWidget = nullptr;

	// ===== 사운드 에셋 =====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound") USoundBase* ClearBGM = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound") USoundBase* BossPhase2BGM = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound") UAudioComponent* BGMComp = nullptr;

	// ===== 보스 이벤트 수신 =====
	UFUNCTION() void OnBossHealthChanged(float Current, float Max, float Percent);
	UFUNCTION() void OnBossDied(AActor* DeadEnemy);

	// ===== 점수판 입력 핸들러 =====
	void ShowScoreboard();
	void HideScoreboard();

private:
	// ===== 진행 상태 =====
	int32 KilledEnemyCount = 0;
	int32 TotalEnemyCount  = 0;
	bool  bGameOverShown   = false;

	FText PendingGameOverReason = NSLOCTEXT("GameOver", "DefaultReason", "게임 오버");

	// ===== 캐시 =====
	UPROPERTY() UUWCrosshairWidget*   CachedCrosshair = nullptr;
	UPROPERTY() USTMovementComponent* CachedMoveComp  = nullptr;
	bool bPrevZoomState = false;

	// ===== 무기 상태 =====
	UPROPERTY() FString CurrentWeaponName;
	bool  bIsShotgunWeapon = false;
	float DamageTextScreenRadiusPx = 60.f;

	// ===== 타이머 핸들 / 딜레이 =====
	FTimerHandle GameOverTimerHandle;
	float GameOverDelay = 1.5f;

	FTimerHandle GameClearTimerHandle;
	float GameClearDelay = 1.5f;

	FDelegateHandle ActorSpawnedHandle;

	// ===== 내부 로직 =====
	void GetPlayerResults(
		int32& OutScore,
		int32& OutKillCount,
		int32& OutDamageDealt,
		int32& OutDamageTaken,
		int32& BulletUesd,
		int32& OutHighScore
	) const;

	UFUNCTION() void RefreshMissionProgress(int32 ProgressIndex);
	UFUNCTION() void HandleEnemyDamageTaken(AActor* DamagedActor, float DamageAmount, bool bCritical);
	UFUNCTION() void HandlePlayerHealed(float HealAmount);
	UFUNCTION() void HandlePlayerDamaged(float DamageAmount);
	UFUNCTION() void ShowDamageNumberAtActor(AActor* Target, int32 Damage, bool bCritical, FName SocketName = TEXT("HealthBar"));

	// 레벨 이동
	UFUNCTION(BlueprintCallable) void LoadLevelWithDataResetAndLoadingScreen(const EStageType& NextStage);

	// 스케줄러
	UFUNCTION() void ScheduleGameOver(float DelaySeconds);
	UFUNCTION() void ScheduleGameClear(float DelaySeconds);

	// 샷건용 데미지텍스트 오프셋
	FVector GetShotgunScreenRandomLoc(const FVector& BaseWorldLoc) const;

	// 현재 보스
	UPROPERTY() ASTEnemyBoss* CurrentBoss = nullptr;
};