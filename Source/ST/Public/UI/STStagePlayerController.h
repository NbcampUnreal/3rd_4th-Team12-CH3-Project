#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "System/STGameTypes.h"
#include "STStagePlayerController.generated.h"

class USTStageWidget;
class USTPauseMenuWidget;
class USTScoreboardWidget;
class USTGameOverWidget;
class USTGameClearWidget;
class UUserWidget;
class UUWCrosshairWidget;
class USTMovementComponent;

UCLASS()
class ST_API ASTStagePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASTStagePlayerController();
	
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	// UI 업데이트 함수
	UFUNCTION() void UpdateHealth(float CurrentHP, float MaxHP);
	UFUNCTION() void UpdateWeapon(const FString& WeaponName);
	UFUNCTION()	void UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo);
	UFUNCTION( BlueprintCallable, Category="Timer")	void UpdateTimer(int32 RemainingSeconds);
	UFUNCTION()	void UpdateEnemyStatus(int32 Killed, int32 Total);
	UFUNCTION()	void AddDamageKillLog(const FString& LogText);

	// 전투 피드백
	UFUNCTION() void ShowHitMarker();
	UFUNCTION() void ShowKillConfirmed();
	UFUNCTION() void ShowDamageTextAt(FVector WorldLocation, int32 Damage);
	UFUNCTION() void HandleEnemyDied_ShowConfirm(AActor* DeadEnemy);

	// 일시정지 메뉴
	void TogglePauseMenu();

	// 메뉴 스테이지 전환
	UFUNCTION()	void HandlePauseReturnToMain();
	UFUNCTION()	void HandleQuitGame();
	UFUNCTION()	void HandleStageClear();	// JM 스테이지 클리어시 델리게이트
	UFUNCTION()	void HandleStageFailed();	// JM 스테이지 실패시 델리게이트

	// Game Over / Clear 버튼
	UFUNCTION()	void HandleGameOverRetry();
	UFUNCTION()	void HandleGameOverReturnToMain();
	UFUNCTION()	void HandleGameClearRetry();
	UFUNCTION()	void HandleGameClearReturnToMain();
	
	// Game Over / Clear UI
	UFUNCTION()
	void ShowGameOverResult(int32 Score, int32 KillCount, int32 DamageDealt, int32 DamageTaken);
	UFUNCTION()
	void ShowGameClearResult();

	// 로딩 화면
	UFUNCTION( BlueprintImplementableEvent )
	void LoadNextStage_BP(EStageType NextStage, int32 LoadingScreenIndex);

	
protected:
	
	// 컴포넌트/위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> StageWidgetClass;

	UPROPERTY()
	USTStageWidget* StageWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;

	UPROPERTY()
	USTPauseMenuWidget* PauseMenuWidget;

	// 점수판 UI 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USTScoreboardWidget> ScoreboardWidgetClass;

	// 점수판 위젯 인스턴스
	UPROPERTY()
	USTScoreboardWidget* ScoreboardWidget;

	// 게임 오버 UI 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USTGameOverWidget> GameOverWidgetClass;

	// 게임 오버 UI 인스턴스
	UPROPERTY()
	USTGameOverWidget* GameOverWidget;

	// 게임 클리어 UI 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USTGameClearWidget> GameClearWidgetClass;

	// 게임 클리어 UI 인스턴스
	UPROPERTY()
	USTGameClearWidget* GameClearWidget;

	// 키 입력 핸들러
	void ShowScoreboard();
	void HideScoreboard();

private:
	int32 KilledEnemyCount = 0;
	int32 TotalEnemyCount  = 0;
	
	UFUNCTION()
	void RefreshMissionProgress(int32 ProgressIndex);
	
	UFUNCTION()
	void HandleEnemyDamageTaken(AActor* DamagedActor, float DamageAmount, bool bCritical);
	
	UFUNCTION()
	void ShowDamageNumberAtActor(AActor* Target, int32 Damage, bool bCritical, FName SocketName = TEXT("HealthBar"));

	UFUNCTION()
	void TriggerGameOverWithTempData();
	UFUNCTION()
	void TriggerGameClearWithTempData();

	// 게임 오버 화면 딜레이
	UFUNCTION()
	void ScheduleGameOver(float DelaySeconds);
	FTimerHandle GameOverTimerHandle;
	float GameOverDelay = 1.5f;
	
	FDelegateHandle ActorSpawnedHandle;
	
	UPROPERTY()
	UUWCrosshairWidget* CachedCrosshair = nullptr;
	
	UPROPERTY()
	USTMovementComponent* CachedMoveComp = nullptr;
	
	bool bPrevZoomState = false;
	bool bGameOverShown = false;
	bool bGameClearShown = false;
};