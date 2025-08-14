// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/STItemBase.h"
#include "STHealItem.generated.h"

/**
 * 
 */
UCLASS()
class ST_API ASTHealItem : public ASTItemBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category="HealAmount")
	float HealAmount;
protected:
	virtual void UseItem(ASTPlayerCharacter* InInstigator) override;


	
};
