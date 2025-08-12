// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STWeaponManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponEquip, const FText&, WeaponName);
// 탄약 변경 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponAmmoChange, int32, CurrentAmmo, int32, MaxAmmo);

enum class EViewMode : uint8;
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
	
protected:
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	UFUNCTION()
	void UpdateWeaponVisibility(EViewMode NewMode);
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
	void StartAiming();
	void StopAiming();
	void ReloadAmmo();
#pragma endregion 

#pragma region DelegateChaning  // ui에 무기 데이터 전달을 위한 delegate chain필요 
public:
	FOnWeaponEquip EquipDelegate;
	FOnWeaponAmmoChange AmmoChangeDelegate;
protected:
	UFUNCTION()
	void OnWeaponEquipped(const FText& WeaponName);

	UFUNCTION()
	void OnWeaponAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo);

	
#pragma endregion 
};
