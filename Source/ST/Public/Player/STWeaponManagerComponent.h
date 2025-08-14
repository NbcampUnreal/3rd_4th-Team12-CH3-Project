// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/STItemPivotData.h"
#include "STWeaponManagerComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponEquip, const FString&, WeaponName);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponEquipChanged, const TSoftClassPtr<ASTWeaponBase>, EquippedWeapon);
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
	void EquipWeapon( TSoftClassPtr<ASTWeaponBase> WeaponClass);
	void UnequipWeapon();
	void RequestEquipWeapon(TSoftClassPtr<ASTWeaponBase> WeaponClass);
	EWeaponType GetCurrentWeaponType();
protected:
	virtual void BeginPlay() override;
	
	void OnEquipMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
	FStItemPivotData* GetWeaponPivotData(EWeaponType type);
	
	void UpdateWeaponSocketOffset(EWeaponType type);
	
	UFUNCTION()
	void UpdateWeaponVisibility(EViewMode NewMode);
	bool CanFireWeapon();

private:
	UPROPERTY()
	TObjectPtr<ASTPlayerCharacter> OwnerChar;
	
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<ASTWeaponBase> CurrentWeapon;

	TSoftClassPtr<ASTWeaponBase> CurrentWeaponClass;

	TSoftClassPtr<ASTWeaponBase> PendingWeaponClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SocketOffset", meta = (AllowPrivateAccess = true))
	TObjectPtr<UDataTable> SocketOffsetTable;
	
	UPROPERTY(EditDefaultsOnly, Category = "Socket")
	FName AttachSocket1P = TEXT("WeaponSocket1P");

	UPROPERTY(EditDefaultsOnly, Category = "Socket")
	FName AttachSocket3P = TEXT("WeaponSocket3P");

	bool bIsWeaponChanged =false;
	
	FOnMontageEnded MontageEndedDelegate;
	


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
	FOnWeaponEquipChanged EquipChangedDelegate;
protected:
	UFUNCTION()
	void OnWeaponEquipped(const FString& WeaponName);

	UFUNCTION()
	void OnWeaponAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo);

	
#pragma endregion 
};
