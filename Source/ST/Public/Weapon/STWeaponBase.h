// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STWeaponDataAsset.h"
#include "STWeaponBase.generated.h"

// 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponEquippedSignature, const FString&, WeaponName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChangedSignature, int32, CurrentAmmo, int32, MagazineSize);


UCLASS(Abstract) // 자식 클래스를 통해서만 사용하도록 Abstract 키워드 추가
class ST_API ASTWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:
	// ========== 컴포넌트 ==========
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	UStaticMeshComponent* WeaponMesh3p;

	// ========== 생성자 ==========
	ASTWeaponBase();

	// ========== 변수 ==========
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	USTWeaponDataAsset* WeaponDataAsset;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Screen Shake")
	TSubclassOf<UCameraShakeBase> FireCameraShake;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|VFX")
	UParticleSystem* MuzzleFlashEffect; // 총구 화염 효과

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|VFX")
	UParticleSystem* ImpactEffect_Default; // 기본 피격 효과 (벽, 바닥 등)

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* ReloadSound;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Delegates")
	FOnWeaponEquippedSignature OnWeaponEquipped;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Delegates")
	FOnAmmoChangedSignature OnAmmoChanged;

	// ========== 함수 ==========
	UFUNCTION(BlueprintPure, Category = "Weapon|Components")
	UStaticMeshComponent* GetWeaponMesh1P() const { return WeaponMesh; }

	UFUNCTION(BlueprintPure, Category = "Weapon|Components")
	UStaticMeshComponent* GetWeaponMesh3P() const { return WeaponMesh3p; }
	
	EFireMode GetFireMode() const;
	
	void Fire();
	void StartFire();
	void StopFire();
	
	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetCurrentAmmo() const { return CurrentAmmo; }

	void ToggleFireMode();
	void StartReload();
	void FinishReload();
	void StartAiming();
	void StopAiming();

	bool IsFiring() const;
	bool IsReloading() const { return bIsReloading; }
	
protected:
	// ========== protected 변수 ==========
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	FString WeaponName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float FireRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 MagazineSize;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ReloadTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CurrentAmmo;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float SpreadAngle;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 PelletsPerShot;
	
	bool bIsAiming = false;
	float DefaultSpreadAngle = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Screen Shake")
	float CameraShakeScale = 1.0f;
	
	FTimerHandle FireRateTimerHandle;
	FTimerHandle ReloadTimerHandle;
	FTimerHandle AutoFireTimerHandle;

	bool bCanFire = true;
	bool bIsReloading = false;

	// ========== protected 함수 ==========
	virtual void BeginPlay() override;
	
	void EnableFire();
	void HandleFire();
	void PlayFireCameraShake();

	// ★★★ 핵심: 자식 클래스가 반드시 구현해야 할 '발사' 기능 선언 ★★★
	virtual void FireWeapon();
};