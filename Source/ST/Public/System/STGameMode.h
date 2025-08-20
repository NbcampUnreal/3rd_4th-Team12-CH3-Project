#pragma once

#include "CoreMinimal.h"
#include "StageInfoData.h"
#include "GameFramework/GameMode.h"
#include "STGameTypes.h"
#include "STGameMode.generated.h"

/* Event Delegate */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStageClear);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStageFailed);

/**
 *  게임 규칙, 결과 판단, 진행을 담당하는 GameMode (작성자 : JM)
 */
UCLASS()
class ST_API ASTGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ASTGameMode();

	UPROPERTY (BlueprintAssignable, Category = "Event" )
	FOnStageClear OnStageClear;		// 스테이지 클리어 이벤트

	UPROPERTY( BlueprintAssignable, Category = "Event" )
	FOnStageFailed OnStageFailed;	// 스테이지 실패 이벤트

	UFUNCTION( BlueprintCallable )
	void OnEnemyKilled();			// 적 처치 시 호출(Enemy -> GameMode)

	UFUNCTION( BlueprintCallable )
	void OnPlayerDied();			// 플레이어 사망 시 호출(PlayerCharacter -> GameMode)
	
protected:
	/* Player */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "PlayerClasses" )
	TSubclassOf<APawn> MainPlayerClass;

	/* Engine Overrides */
	virtual void BeginPlay() override;
	virtual void RestartPlayer(AController* NewPlayer) override;

	/* Stage Data */
	UPROPERTY(EditDefaultsOnly, Category="StageInfo", meta = (AllowPrivateAccess = "true"))
	UDataTable* StageInfoTable;
	

private:
	/* Stage Runtime Variables */
	UPROPERTY( VisibleInstanceOnly, Category = "StageInfo" )
	int32 TotalEnemies = 0;

	UPROPERTY( VisibleInstanceOnly, Category = "StageInfo" )
	int32 DeadEnemies = 0;

	UPROPERTY( VisibleInstanceOnly, Category = "StageInfo" )
	int32 StageTimeLimit = 0;
	bool bStageCleared = false;
	
	FTimerHandle StageTimerHandle;
	FTimerHandle StageTimerUpdateHandle;

	/* Stage Logic */
	void StartStage();
	void EndStage(const EStageResult Result);
	void ResetStage();
	void OnTimeOver();

	
	/* Stage Event Handlers */
	UFUNCTION()
	void HandlePlayerEnteredClearZone();
	
	void BindStageClearZoneEnterEvent();
	void BindPlayerDeathEvent();

	/* Stage Utilities */
	void SetStagePhase(const EStagePhase NewPhase) const;
	FStageInfoRow* GetStageInfoFromDataTable(const FString& StageName) const;
	void UpdateStageTimerUI() const;
};
