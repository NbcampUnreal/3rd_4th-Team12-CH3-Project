#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "STGameTypes.h"
#include "STGameInstance.generated.h"

/**
 *	싱글톤, 게임 종료시까지 유지됨
 *	모든 레벨 이동은 GameInstance의 GoToLevel(LevelName)을 사용하기
 */
UCLASS()
class ST_API USTGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	USTGameInstance();

	/* member variables */
	FPlayerStateInfo PlayerStateInfo;		// TODO: 구조체로 플레이어 체력 등 전달하기
	
	EStageType LastStage;
	ECharacterType SelectedCharacter;
	
	/* member functions */
	UFUNCTION( BlueprintCallable, Category="Level" )
	void StartNewGame();

	UFUNCTION( BlueprintCallable, Category="Level" )
	void GoToNextStage();
	
	UFUNCTION( BlueprintCallable, Category="Level" )
	void GoToLevel(EStageType StageType);
	
	UFUNCTION( BlueprintCallable, Category="Level" )
	void GoToMainMenu();

	UFUNCTION( BlueprintCallable, Category="Level" )
	void QuitGame();

	UFUNCTION( BlueprintCallable, Category="Save" )
	void SetPlayerStateInfo(const FPlayerStateInfo& NewInfo);

	UFUNCTION( BlueprintCallable, Category="Save" )
	const FPlayerStateInfo& GetPlayerStateInfo() const;

	// TODO: 게임 저장/불러오기(아카이빙) 기능 구현
	
private:
	EStageType GetNextStageType(EStageType CurrentStage) const;
	void ResetGameData();
};
