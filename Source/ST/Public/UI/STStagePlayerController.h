#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "STStagePlayerController.generated.h"

class USTStageWidget;
class USTPauseMenuWidget;
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

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> StageWidgetClass;

	UPROPERTY()
	USTStageWidget* StageWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;

	UPROPERTY()
	USTPauseMenuWidget* PauseMenuWidget;
};