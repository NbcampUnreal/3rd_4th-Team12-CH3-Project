// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STWeaponManagerComponent.generated.h"

enum class EWeaponType : uint8;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponEquip, const FText&, WeaponName);
// 탄약 변경 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponAmmoChange, int32, CurrentAmmo, int32, MaxAmmo);

enum class EViewMode : uint8;
class ASTPlayerCharacter;
class ASTWeaponBase;
struct FStItemPivotData;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ST_API USTWeaponManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USTWeaponManagerComponent();
	//equip
	void EquipWeapon(TSubclassOf<ASTWeaponBase> WeaponClass);
	//unequip
	void UnequipWeapon();

	void RequestEquipWeapon(TSubclassOf<ASTWeaponBase> WeaponClass);
	
protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void UpdateWeaponVisibility(EViewMode NewMode);

	void OnEquipMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	FStItemPivotData* GetWeaponPivotData(EWeaponType type);

	void UpdateWeaponSocketOffset(EWeaponType type);
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

	UPROPERTY(EditDefaultsOnly, Category = "SocketOffsetTable")
	TObjectPtr<UDataTable> SocketOffsetTable;
	
	TSubclassOf<ASTWeaponBase> PendingWeaponClass;
	
	FOnMontageEnded MontageEndedDelegate;

	bool bIsWeaponChanged = false;

	bool CanFireWeapon();

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
