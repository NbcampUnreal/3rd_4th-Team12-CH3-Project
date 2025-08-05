// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Weapon/STWeaponType.h"
#include "STWeaponDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class ST_API USTWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category="Weapon Data")
	FSTWeaponData WeaponData;
};
