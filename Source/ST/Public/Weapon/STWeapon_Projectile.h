// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/STWeaponBase.h"
#include "STWeapon_Projectile.generated.h"

class ASTProjectile;

UCLASS()
class ST_API ASTWeapon_Projectile : public ASTWeaponBase
{
	GENERATED_BODY()

public:
	// 발사할 투사체 클래스를 이 클래스 전용 변수로 직접 가집니다.
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class ASTProjectile> ProjectileClass;

	// 투사체를 총구보다 얼마나 앞에서 생성할지 정하는 거리 값 (cm 단위)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float SpawnOffset = 35.0f;

	
protected:
	// 부모 클래스의 'FireWeapon' 함수를 '투사체 발사 방식'으로 구현합니다.
	virtual void FireWeapon() override;
};