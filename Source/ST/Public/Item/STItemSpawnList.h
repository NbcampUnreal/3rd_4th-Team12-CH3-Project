// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "STItemSpawnList.generated.h"

class ASTItemBase;
/**
 * 
 */
UCLASS(BlueprintType)
class ST_API USTItemSpawnList : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSoftClassPtr<ASTItemBase>> SpawnableItems;
	
};
