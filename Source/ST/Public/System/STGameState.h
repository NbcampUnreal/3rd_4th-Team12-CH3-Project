#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "STGameTypes.h"
#include "STGameState.generated.h"

/* Event Delegate */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemainingTimeUpdated, int32, NewRemainingTime);

/**
 *  게임 진행 상태를 저장, 전파
 *  GameMode에서만 Setter 호출(friend 키워드)
 *  다른 곳에서는 읽기전용 
 */
UCLASS()
class ST_API ASTGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	ASTGameState();

	/* 현재 게임 상태정보 (읽기 전용) */
	UFUNCTION( BlueprintPure )
	const FGameStateInfo& GetGameStateInfo() const;		// 상수 참조를 통해 불필요한 복사 방지

	UPROPERTY( BlueprintAssignable, Category="Event" )
	FOnRemainingTimeUpdated OnRemainingTimeUpdated;		// 남은시간 업데이트 이벤트

	
protected:
	friend class ASTGameMode;	// ASTGameMode 클래스에게 protected 멤버 접근을 허용

	/* 현재 게임 진행 상태 */
	UPROPERTY ( BlueprintReadOnly, Category = "GameState" )
	FGameStateInfo GameStateInfo;

	/* Setters : GameMode만 접근 가능 */
	void SetStagePhase(EStagePhase NewStagePhase);
	void SetStageResult(EStageResult NewStageResult);
	void SetRemainingEnemies(int32 NewRemainingEnemies);
	void SetRemainingTime(int32 NewRemainingTime);
	void SetBossPhase(int32 NewBossPhase);
	void SetStageGoalText(const FText& NewStageGoalText);
	void SetStageProgressList(const TArray<FText>& NewStageProgressList);
};
