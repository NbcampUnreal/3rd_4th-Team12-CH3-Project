#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "STGameTypes.h"
#include "STGameState.generated.h"

/**
 *  게임 진행 상태를 저장, 전파
 *  GameMode에서만 Setter 호출, 다른 곳에서는 읽기만 하기 (friend 키워드)
 */
UCLASS()
class ST_API ASTGameState : public AGameState
{
	GENERATED_BODY()
public:
	ASTGameState();
	UFUNCTION( BlueprintCallable )
	EStagePhase GetStagePhase() const;
	UFUNCTION( BlueprintCallable )
	int32 GetRemainingEnemies() const;
	UFUNCTION( BlueprintCallable )
	float GetRemainingTime() const;
	UFUNCTION( BlueprintCallable )
	int32 GetBossPhase() const;
	UFUNCTION( BlueprintCallable )
	EStageResult GetStageResult() const;
			

protected:
	friend class ASTGameMode;	// ASTGameMode 클래스에게 protected 멤버 접근을 허용
	
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "GameState" )
	EStagePhase StagePhase;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "GameState" )
	int32 RemainingEnemies;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "GameState" )
	float RemainingTime;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "GameState" )
	int32 BossPhase;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "GameState" )
	EStageResult StageResult;


	UFUNCTION( BlueprintCallable )
	void SetStagePhase(EStagePhase NewPhase);

	UFUNCTION( BlueprintCallable )
	void SetRemainingEnemies(int32 NewCount);

	UFUNCTION( BlueprintCallable )
	void SetRemainingTime(float NewTime);
	
	UFUNCTION( BlueprintCallable )
	void SetBossPhase(int32 NewPhase);

	UFUNCTION( BlueprintCallable )
	void SetStageResult(EStageResult NewResult);
};
