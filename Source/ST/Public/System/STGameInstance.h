#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "STGameTypes.h"
#include "STGameInstance.generated.h"

class USTPlayerBaseData;

/**
 *	싱글톤, 게임 종료시까지 유지됨
 *	모든 레벨 이동은 GameInstance의 GoToLevel(LevelName)을 사용하기
 *	 - 작성자 : JM
 */
UCLASS()
class ST_API USTGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	USTGameInstance();

	virtual void Init() override;

	/* Player Info */
	UPROPERTY( BlueprintReadWrite, Category="PlayerInfo" )
	ECharacterType SelectedCharacter;

	UPROPERTY()
	APawn* PlayerPawnInstance = nullptr;

	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category="PlayerClasses" )
	TSubclassOf<APawn> MainPlayerClass;

	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category="PlayerMeshs" )
	USkeletalMesh* JaxMercerCharacterMesh;		// Blueprint에서 메시 할당

	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category="PlayerMeshs" )
	USkeletalMesh* AvaRainesCharacterMesh;		// Blueprint에서 메시 할당

	UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, Category="PlayerDefaultDataAsset")
	TObjectPtr<USTPlayerBaseData> PlayerDefaultDataAsset; // 캐릭터 기본 데이터 

	/* Level Info */
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Level" )	// JM: 디버그용
	EStageType LastStage;

	/* Player State */
	UPROPERTY()
	FPlayerStateInfo PlayerStateInfo;
	
	UFUNCTION( BlueprintCallable, Category="Save" )
	const FPlayerStateInfo& GetPlayerStateInfo() const;

	UFUNCTION( BlueprintCallable, Category="Save" )
	void SetPlayerStateInfo(const FPlayerStateInfo& NewInfo);

	/* Level Control */
	UFUNCTION( BlueprintCallable, Category="Level" )
	void GoToLevel(const EStageType StageType);
	
	UFUNCTION( BlueprintCallable, Category="Level" )
	void GoToMainMenu();

	UFUNCTION( BlueprintCallable, Category="Level" )
	void QuitGame();

	/* Retry */
	UFUNCTION( BlueprintCallable, Category="Retry" )
	void ResetDataForRetry();

	UFUNCTION( BlueprintCallable, Category="Retry" )
	void GoToRetry();
	
	/* Save/Load */
	void LoadSavedData();
	void SaveSavedData(const FSaveData& SaveData);	// 데이터는 프로젝트 폴더 > Saved > SaveGames에 저장됨(배포후엔 C:\\... AppData/프로젝트명/Saved/SaveGames에 저장)


	
	/* Unused... */
	UFUNCTION( BlueprintCallable, Category="Level" )
	void StartNewGame();

	UFUNCTION( BlueprintCallable, Category="Level" )
	void GoToNextStage();
	
private:
	EStageType GetNextStageType(EStageType CurrentStage) const;

	void ResetGameData();
	void ResetPlayerStateInfo();
};
