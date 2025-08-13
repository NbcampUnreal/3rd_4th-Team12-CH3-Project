// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/STHealItem.h"

#include "Player/STHealthComponent.h"
#include "Player/STPlayerCharacter.h"

void ASTHealItem::UseItem(ASTPlayerCharacter* InInstigator)
{
	Super::UseItem(InInstigator);

	if (IsValid(InInstigator->GetHealthComponent()))
	{
		InInstigator->GetHealthComponent()->Heal(HealAmount);
	}
	DestroyedItem();
	
}
