#pragma once

#include "CoreMinimal.h"
#include "StageInfoData.h"
#include "GameFramework/GameMode.h"
#include "STGameTypes.h"
#include "STGameMode.generated.h"

// DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerEnteredClearZone);	// Clear Zone -> PlayerController -> GameMode 순으로 접근
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStageClear);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStageFailed);

/**
 *  게임 규칙, 결과 판단, 진행
 */
UCLASS()
class ST_API ASTGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ASTGameMode();

	UPROPERTY (BlueprintAssignable, Category = "Event" )
	FOnStageClear OnStageClear;

	UPROPERTY( BlueprintAssignable, Category = "Event" )
	FOnStageFailed OnStageFailed;

	UFUNCTION( BlueprintCallable )
	void OnEnemyKilled();			// Enemy -> GameMode

	UFUNCTION( BlueprintCallable )
	void OnPlayerDied();			// PlayerCharacter -> GameMode
	
protected:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "PlayerClasses" )
	TSubclassOf<APawn> MainPlayerClass;
	
	virtual void BeginPlay() override;
	virtual void RestartPlayer(AController* NewPlayer) override;

	
	

private:
	// member variables
	UPROPERTY( VisibleInstanceOnly, Category = "StageInfo" )
	int32 TotalEnemies;
	UPROPERTY( VisibleInstanceOnly, Category = "StageInfo" )
	int32 DeadEnemies;
	UPROPERTY( VisibleInstanceOnly, Category = "StageInfo" )
	int32 StageTimeLimit;
	FTimerHandle StageTimerHandle;
	FTimerHandle StageTimerUpdateHandle;
	bool bStageCleared;
	UPROPERTY(EditDefaultsOnly, Category="StageInfo", meta = (AllowPrivateAccess = "true"))
	UDataTable* StageInfoTable;
	
	// member functions
	void StartStage();
	void EndStage(const EStageResult Result);
	void ResetStage();
	void OnTimeOver();
	UFUNCTION()
	void HandlePlayerEnteredClearZone();
	void SetStagePhase(const EStagePhase NewPhase) const;
	FStageInfoRow* GetStageInfoFromDataTable(const FString& StageName) const;
	void BindStageClearZoneEnterEvent();
	void BindPlayerDeathEvent();
	void UpdateStageTimerUI();
};
