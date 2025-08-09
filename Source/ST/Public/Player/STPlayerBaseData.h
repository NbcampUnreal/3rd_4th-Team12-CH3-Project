// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "STPlayerBaseData.generated.h"

/**
 * 
 */
UCLASS()
class ST_API USTPlayerBaseData : public UDataAsset
{
	GENERATED_BODY()
public:
	// Health
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="Health")
	float BaseMaxHealth;


	//Move
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="Movement")
	float BaseWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="Movement")
	float SprintMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="Movement")
	float CrouchMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category="Movement")
	float ZoomMultiplier;
	
};
