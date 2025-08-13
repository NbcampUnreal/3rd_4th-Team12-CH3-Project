// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Engine/DataTable.h" 
#include "StItemPivotData.generated.h" 

enum class EWeaponType : uint8;

USTRUCT(BlueprintType)
struct FStItemPivotData :public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(Editanywhere, BlueprintReadOnly)
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector PositionOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RotationOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector FPSPositionOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator FPSRotationOffset;
	
};