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
	// FPlayerStateInfo PlayerStateInfo;		// TODO: 구조체로 플레이어 체력 등 전달하기
	
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

	// UFUNCTION( BlueprintCallable, Category="Save" )
	// void UpdatePlayerStateInfo(const FPlayerStateInfo& NewInfo);

	// TODO: 게임 저장/불러오기(아카이빙) 기능 구현
	
private:
	EStageType GetNextStageType(EStageType CurrentStage) const;
	void ResetGameData();


	/*
	UPROPERTY( BlueprintReadWrite, Category="Profile" )		// 보통 Edit/VisibleAnywhere을 거의 쓰지 않음(의미 없음).
	ECharacterType SelectedCharacter;						// GameInstance는 런타임에 자동 생성되는 글로벌 객체라서, 레벨에 배치불가 + 에디터에서 직접 선택/편집 불가

	UPROPERTY( BlueprintReadWrite, Category="Progress" )
	EStageType LastStage;

	UPROPERTY( BlueprintReadWrite, Category="Progress" )
	bool bIsContinue;

	// TODO: 설정 옵션 확장 가능(마스터 볼륨) 
	/*UPROPERTY( BlueprintReadWrite, Category="Settings" )
	float MasterVolume;#1#

	
	UFUNCTION( BlueprintCallable, Category="Save" )
	void SaveGameData();

	UFUNCTION( BlueprintCallable, Category="Save" )
	void LoadGameData();

	UFUNCTION( BlueprintCallable, Category="Init" )
	void ResetGame();

	UFUNCTION( BlueprintCallable, Category="Level" )
	void GoToLevel(EStageType StageType);

	UFUNCTION( BlueprintCallable, Category="Level" )
	void LoadMainMenuLevel();

	UFUNCTION( BlueprintCallable, Category="Level" )
	void StartNewGame();

	UFUNCTION( BlueprintCallable, Category="Level" )
	void StartStage1();

	UFUNCTION( BlueprintCallable, Category="Level" )
	void StartContinuousGame();

	UFUNCTION( BlueprintCallable, Category="Level" )
	void QuitGame();*/
};
