#pragma once

#include "CoreMinimal.h"
#include "STGameTypes.h"
#include "GameFramework/SaveGame.h"
#include "STSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class ST_API USTSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY( BlueprintReadWrite, Category="SaveData" )
	FSaveData SaveData;

	// 슬롯 기본값
	static const FString DefaultSlotName;
	static const int32 DefaultUserIndex;
};
