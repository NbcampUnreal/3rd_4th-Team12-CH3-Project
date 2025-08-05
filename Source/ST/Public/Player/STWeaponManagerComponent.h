// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STWeaponManagerComponent.generated.h"


class ASTWeaponBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ST_API USTWeaponManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	USTWeaponManagerComponent();
	void EquipWeapon(TSubclassOf<ASTWeaponBase> WeaponClass);
	void UnequipWeapon();
	void UpdateWeaponVisibility();
private:
	UPROPERTY()
	TObjectPtr<ASTWeaponBase> CurrentWeapon;

	UPROPERTY(EditDefaultsOnly)
	FName AttachSocket1P;

	UPROPERTY(EditDefaultsOnly)
	FName AttachSocket3P;

		
};
