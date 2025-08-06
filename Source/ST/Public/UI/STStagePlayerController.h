#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "STStagePlayerController.generated.h"

class USTStageWidget;
class UUserWidget;

UCLASS()
class ST_API ASTStagePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASTStagePlayerController();
	virtual void BeginPlay() override;

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

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> StageWidgetClass;

	UPROPERTY()
	USTStageWidget* StageWidget;
};