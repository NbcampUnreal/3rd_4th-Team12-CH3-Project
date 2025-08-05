// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STWeaponManagerComponent.generated.h"


class ASTPlayerCharacter;
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
protected:
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
private:
	UPROPERTY()
	TObjectPtr<ASTPlayerCharacter> OwnerChar;
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<ASTWeaponBase> CurrentWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DefaultWeapon", meta = (AllowPrivateAccess = true))
	TSubclassOf<ASTWeaponBase> DefaultWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Socket")
	FName AttachSocket1P = TEXT("WeaponSocket1P");

	UPROPERTY(EditDefaultsOnly, Category = "Socket")
	FName AttachSocket3P = TEXT("WeaponSocket3P");


#pragma region  Input
public:
	void StartFire();
	void StopFire();
	void Reload();
	void ReloadAmmo();
#pragma endregion 

		
};
