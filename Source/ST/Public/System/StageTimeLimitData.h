#pragma once

#include "CoreMinimal.h"
#include "StageTimeLimitData.generated.h"
/**
 * 
 */
USTRUCT( BlueprintType )
struct FStageTimeLimitRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	FString StageName;

	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	float TimeLimit;
	
};
