// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/STWeaponItem.h"

#include "Player/STPlayerCharacter.h"
#include "Player/STWeaponManagerComponent.h"
#include "Weapon/STWeaponBase.h"

void ASTWeaponItem::UseItem(ASTPlayerCharacter* InInstigator)
{
	Super::UseItem(InInstigator);

	if (!WeaponClass.IsNull())
	{
		UClass* WeaponClassToSpawn = WeaponClass.LoadSynchronous();
		if (WeaponClassToSpawn)
		{
			InInstigator->GetWeaponManagerComponent()->RequestEquipWeapon(WeaponClassToSpawn);
		}
	}
	DestroyedItem();
}
