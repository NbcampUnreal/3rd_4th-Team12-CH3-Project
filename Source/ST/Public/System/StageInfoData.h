#pragma once

#include "CoreMinimal.h"
#include "StageInfoData.generated.h"
/**
 * 
 */
USTRUCT( BlueprintType )
struct FStageInfoRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	FString StageName;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	int32 TimeLimit;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	FText StageGoalText;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	TArray<FText> StageProgressList;
};
