#pragma once

#include "CoreMinimal.h"
#include "STGameTypes.h"
#include "GameFramework/PlayerState.h"
#include "STPlayerState.generated.h"

/**
 *  플레이어의 개별 정보 저장
 */
UCLASS()
class ST_API ASTPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ASTPlayerState();

	/*중복선언 에러*/
	/*UPROPERTY( BlueprintReadWrite, Category="PlayerState" ) 
	int32 Score;*/

	UPROPERTY( BlueprintReadWrite, Category="PlayerState" )
	int32 KillCount;

	UPROPERTY( BlueprintReadWrite, Category="PlayerState" )
	ECharacterType SelectedCharacter;

	UFUNCTION( BlueprintCallable )
	void ResetPlayerState();
};
