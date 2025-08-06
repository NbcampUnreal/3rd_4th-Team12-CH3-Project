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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="TPS")
	TObjectPtr<UAnimMontage> ReloadMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="TPS")
	TObjectPtr<UAnimMontage> DeadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="FPS")
	TObjectPtr<UAnimMontage> FPSReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="FPS")
	TObjectPtr<UAnimMontage> FPSShootMontage;
	
	
};
