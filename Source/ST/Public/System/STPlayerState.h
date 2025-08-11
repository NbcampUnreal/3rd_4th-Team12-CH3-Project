#pragma once

#include "CoreMinimal.h"
#include "STGameTypes.h"
#include "GameFramework/PlayerState.h"
#include "STPlayerState.generated.h"

/**
 *  플레이어의 개별 정보 저장
 *  (get은 그냥 변수에 접근해도 괜찮은데)
 *  (set은 반드시 setter 로 접근하기)
 */
UCLASS()
class ST_API ASTPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ASTPlayerState();

	/* getters */
	UFUNCTION( BlueprintPure, Category="PlayerState")
	const FPlayerStateInfo& GetPlayerStateInfo() const;	// 상수 참조를 통해 불필요한 복사 방지

	/* setters */ 
	void SetLastStage( EStageType NewStage );
	void SetSelectedCharacter( ECharacterType NewCharacter );

protected:
	virtual void BeginPlay() override;
	
private:
	FPlayerStateInfo PlayerStateInfo;
	
	
	



	/*
	/*중복선언 에러#1#
	/*UPROPERTY( BlueprintReadWrite, Category="PlayerState" ) 
	int32 Score;#1#

	UPROPERTY( BlueprintReadWrite, Category="PlayerState" )
	int32 KillCount;

	UPROPERTY( BlueprintReadWrite, Category="PlayerState" )
	ECharacterType SelectedCharacter;

	UFUNCTION( BlueprintCallable )
	void ResetPlayerState();*/
};
