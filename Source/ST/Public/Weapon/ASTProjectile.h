// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ASTProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class UParticleSystem;
class UAudioComponent;

UCLASS()
class ST_API ASTProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ASTProjectile();

protected:
	// ========== 컴포넌트 ==========
	// 충돌 시 한 번 재생될 사운드
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* ImpactSound;

	// 중력장 효과가 지속되는 동안 반복 재생될 사운드
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* VortexLoopSound;

	UPROPERTY()
	UAudioComponent* VortexAudioComponent;
	
	// 충돌을 감지하는 구체(Sphere) 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComponent;

	// 투사체의 외형을 보여주는 스태틱 메시 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	// 투사체를 날아가게 하는 물리 로직을 담당하는 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovementComponent;

	// ========== 프로퍼티 ==========

	// 충돌 시 생성될 파티클 이펙트
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* ImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Vortex")
	TSubclassOf<AActor> VortexEffectActorClass;

	// 피해량
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float Damage;

	// 광역 피해 반경
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageRadius;


	// ========== 중력탄(Vortex) 효과 프로퍼티 ==========
	// 이 값들은 블루프린트에서 쉽게 수정할 수 있습니다.

	// 중력이 작용할 반경
	UPROPERTY(EditDefaultsOnly, Category = "Vortex")
	float VortexRadius = 300.0f;

	// 적을 끌어당기는 힘의 세기
	UPROPERTY(EditDefaultsOnly, Category = "Vortex")
	float VortexStrength = 2000.0f;

	// 중력 효과가 지속되는 시간
	UPROPERTY(EditDefaultsOnly, Category = "Vortex")
	float VortexDuration = 4.5f;

	// 데미지를 주는 간격 (0.2초마다)
	UPROPERTY(EditDefaultsOnly, Category = "Vortex")
	float DamageTickInterval = 0.2f;

	// 간격마다 주는 데미지
	UPROPERTY(EditDefaultsOnly, Category = "Vortex")
	float DamagePerTick = 5.0f;


	
	// ========== 함수 ==========

	// 게임 시작 시 호출되는 함수
	virtual void BeginPlay() override;

	// 충돌이 발생했을 때 호출될 함수
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// 주기적으로 호출되어 주변 적을 끌어당기고 데미지를 주는 함수
	void ApplyVortexForce();

	// 지속시간이 끝나면 투사체를 파괴하는 함수
	void StopVortexEffect();
	
	// 타이머를 관리하기 위한 핸들
	FTimerHandle VortexTickTimerHandle;
	FTimerHandle VortexDurationTimerHandle;
};