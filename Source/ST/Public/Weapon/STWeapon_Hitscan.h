// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/STWeaponBase.h"
#include "STWeapon_Hitscan.generated.h"

UCLASS()
class ST_API ASTWeapon_Hitscan : public ASTWeaponBase
{
	GENERATED_BODY()

public:
	// 히트스캔 전용 이펙트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|VFX")
	UParticleSystem* TracerEffect;

protected:
	// ========== 핵심 기능 구현 ==========
	// 부모 클래스의 'FireWeapon' 함수를 '라인 트레이스 방식'으로 구체화합니다.
	virtual void FireWeapon() override;

private:
	// ========== 히트스캔 전용 내부 함수 ==========
	// 이 클래스 내부에서만 사용할 발사 및 피격 처리 함수입니다.
	
	// 라인 트레이스를 발사하는 함수
	void PerformTrace(const FVector& Start, const FVector& Direction);

	// 라인 트레이스 충돌 결과를 처리하는 함수
	void ProcessHit(const FHitResult& HitResult);
};