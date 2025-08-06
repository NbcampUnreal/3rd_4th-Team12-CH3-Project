#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "STGameTypes.h"
#include "STGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerEnteredClearZone);

/**
 *  게임 규칙, 결과 판단, 진행
 */
UCLASS()
class ST_API ASTGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ASTGameMode();

	UPROPERTY( BlueprintAssignable, Category = "Event" )
	FOnPlayerEnteredClearZone OnPlayerEnteredClearZone;

	UFUNCTION( BlueprintCallable )
	void StartStage();
	
	UFUNCTION( BlueprintCallable )
	void OnEnemyKilled();
	
	UFUNCTION( BlueprintCallable )
	void OnPlayerDied();
	
	UFUNCTION( BlueprintCallable )
	void SetBossPhase(int32 NewPhase);
	
	void OnTimeOver();
	float GetRemainingTime() const;
	void CheckStageClear();

protected:
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
	
	FTimerHandle StageTimerHandle;


	virtual void BeginPlay() override;
	void EndStage(EStageResult Result);
	void ResetStage();
	float GetStageTimeLimit(FString StageName) const;
	void HandlePlayerEnteredClearZone();

private:
	void SetStagePhase(EStagePhase NewPhase);
};
