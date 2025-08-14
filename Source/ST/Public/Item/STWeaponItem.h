// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/STItemBase.h"
#include "STWeaponItem.generated.h"

class ASTWeaponBase;
/**
 * 
 */
UCLASS()
class ST_API ASTWeaponItem : public ASTItemBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "My Blueprint")
	TSoftClassPtr<ASTWeaponBase> WeaponClass;

	virtual void UseItem(ASTPlayerCharacter* InInstigator) override;
	
};
