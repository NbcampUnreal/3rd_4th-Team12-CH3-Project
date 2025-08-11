#pragma once

#include "CoreMinimal.h"
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
	virtual void BeginPlay() override;

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
	int32 GetStageInfoFromDataTable(const FString& StageName) const;
	void BindStageClearZoneEnterEvent();
	void UpdateStageTimerUI();


	
	
	

/*
public:
	/*UPROPERTY( BlueprintAssignable, Category = "Event" )
		FOnPlayerEnteredClearZone OnPlayerEnteredClearZone;*/
	/*UFUNCTION( BlueprintCallable )
	void StartStage();#1#
protected:
	/* member variables #1#
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "GameRule" )
	int32 TotalEnemies;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "GameRule" )
	int32 DeadEnemies;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "GameRule" )
	int32 BossPhase;
	
	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category = "GameRule" )
	float StageTimeLimit;
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "GameRule" )
	float RemainingTime;

	UPROPERTY( EditDefaultsOnly, Category = "GameRule" )
	UDataTable* StageTimeTable;

	bool bStageCleared;
	
	FTimerHandle StageTimerHandle;

	/* member functions #1#
	UFUNCTION()
	void HandlePlayerEnteredClearZone();
	

	void EndStage(EStageResult Result);
	void ResetStage();
	float GetStageTimeLimit(FString StageName) const;
	/*UFUNCTION( BlueprintCallable )
	void SetBossPhase(int32 NewPhase);
	
	void OnTimeOver();
	float GetRemainingTime() const;
	void CheckStageClear();#1#


private:
	void SetStagePhase(EStagePhase NewPhase);*/
};
