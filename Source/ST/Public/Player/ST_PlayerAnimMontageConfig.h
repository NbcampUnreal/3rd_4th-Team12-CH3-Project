// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ST_PlayerAnimMontageConfig.generated.h"

/**
 * 
 */
UCLASS()
class ST_API UST_PlayerAnimMontageConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> ReloadMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> DeadMontage;
	
	
};
