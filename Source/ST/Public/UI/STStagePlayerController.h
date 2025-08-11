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

UCLASS()
class ST_API ASTStagePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASTStagePlayerController();
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// UI 업데이트 함수
	UFUNCTION()	// NOTE: FROM JM, 이거 에러 안났나요..? UFUNCTION 안붙이면 예외나던데
	void UpdateHealth(float CurrentHP, float MaxHP);
	
	void UpdateWeapon(const FString& WeaponName);
	void UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo);
	void UpdateTimer(int32 RemainingSeconds);
	void UpdateEnemyStatus(int32 Killed, int32 Total);
	void AddDamageKillLog(const FString& LogText);

	// 전투 피드백
	void ShowHitMarker();
	void ShowKillConfirmed();
	void ShowDamageTextAt(FVector WorldLocation, int32 Damage);

	// ESC 메뉴 토글
	void TogglePauseMenu();

	UFUNCTION()
	void HandleQuitGame();
	
	UFUNCTION()
	void HandleStageClear();	// JM 스테이지 클리어시 델리게이트

	UFUNCTION( BlueprintImplementableEvent )
	void LoadNextStage_BP(EStageType NextStage, int32 LoadingScreenIndex);
	

	
	void ShowGameOverResult(int32 Score, int32 KillCount, int32 DamageDealt, int32 DamageTaken);
	void ShowGameClearResult(int32 Score, int32 HighScore);
	
protected:
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

	// [Tab] 키 입력 핸들러
	void ShowScoreboard();
	void HideScoreboard();
};