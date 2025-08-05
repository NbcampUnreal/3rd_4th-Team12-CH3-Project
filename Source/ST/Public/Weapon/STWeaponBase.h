// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STWeaponDataAsset.h"
#include "STWeaponBase.generated.h"

UCLASS()
class ST_API ASTWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:
	//무기 스테틱 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	UStaticMeshComponent* WeaponMesh3p;
	//데이터 에셋에서 값 가져오기
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	USTWeaponDataAsset* WeaponDataAsset;
	
	// 발사 시 재생할 VFX를 블루프린트나 데이터 애셋에서 설정할 수 있도록 노출시킵니다.
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|VFX")
	UParticleSystem* MuzzleFlashEffect; // 총구 화염 효과

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|VFX")
	UParticleSystem* ImpactEffect_Default; // 기본 피격 효과 (벽, 바닥 등)

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|VFX")
	UParticleSystem* ImpactEffect_Flesh; // 캐릭터(살) 피격 효과
    
	//총기 발사 소리
	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundBase* ReloadSound ;

	//생성자
	ASTWeaponBase();

	//총기 발사 모드 함수
	EFireMode GetFireMode() const;
	
	//총알 발사 함수
	void Fire();
	void StartFire();
	void StopFire();
	// ASTWeaponBase.h
	
	// 현재 탄약 수를 반환하는 함수
	UFUNCTION(BlueprintPure, Category = "Weapon")
	int32 GetCurrentAmmo() const { return CurrentAmmo; }


	//발사 모드 전환
	void ToggleFireMode();

	
	//총알 장전 함수
	void StartReload();     // 장전 시작 함수
	void FinishReload();    // 장전 완료 함수

	
protected:


	EWeaponType WeaponType;

	//무기 속성
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
	
	//샷건
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float SpreadAngle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 PelletsPerShot;

	//총알 발사 딜레이
	FTimerHandle FireRateTimerHandle;
	//장전 타이머
	FTimerHandle ReloadTimerHandle;
	//총기 사격 모드 타이머
	FTimerHandle AutoFireTimerHandle;

	//발사 딜레이 설정 함서
	bool bCanFire = true;
	//장전 딜레이
	bool bIsReloading = false;

	//함수
	virtual void BeginPlay() override;
	//연사 조절 함수
	void EnableFire();

	//fire에서 검사 통과후 총괄 함수
	void HandleFire();

	//라인 트레이스 함수
	void PerformTrace(const FVector& Start, const FVector& Direction);

	//라인 트레스 충돌 정보 함수
	void ProcessHit(const FHitResult& HitResult);

	

};
