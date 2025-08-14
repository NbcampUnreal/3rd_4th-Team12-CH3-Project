// Enemy/STEnemyProjectile.h - Action 시스템 전용 (정석적인 초기화)
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "STEnemySkillProjectile.generated.h"

UCLASS()
class ST_API ASTEnemySkillProjectile : public AActor
{
    GENERATED_BODY()
    
public: 
    ASTEnemySkillProjectile();

    UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
    class USphereComponent* CollisionComponent;
    
    UPROPERTY(VisibleAnywhere, Category = "Projectile")
    class UStaticMeshComponent* ProjectileMesh;
    
    UPROPERTY(VisibleAnywhere, Category = "Movement")
    class UProjectileMovementComponent* ProjectileMovement;
    
    UPROPERTY(VisibleDefaultsOnly, Category="VFX")
    UParticleSystemComponent* ProjectileParticleComp;
    
    UPROPERTY(EditDefaultsOnly, Category="VFX")
    UParticleSystem* ProjectileParticle;

    UPROPERTY(VisibleAnywhere, Category="VFX")
    class UNiagaraComponent* ProjectileNiagaraComp;

    // === Action 시스템용 속성들 ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float ProjectileDamage = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float AreaDamageRadius = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
    UParticleSystem* HitEffect = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
    UNiagaraSystem* NiagaraHitEffect = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SFX")
    USoundBase* HitSound = nullptr;

    // === Action에서 설정할 이펙트/사운드 ===
    UPROPERTY(BlueprintReadWrite, Category="Effects")
    UParticleSystem* TrailEffect = nullptr;
    UPROPERTY(BlueprintReadWrite, Category="Effects")
    UNiagaraSystem* NiagaraTrailEffect = nullptr;

    UPROPERTY(BlueprintReadWrite, Category="Effects")
    UParticleSystem* ExplosionEffect = nullptr;
    UPROPERTY(BlueprintReadWrite, Category="Effects")
    UNiagaraSystem* NiagaraExplosionEffect = nullptr;

    UPROPERTY(BlueprintReadWrite, Category="Sound")
    USoundBase* FireSound = nullptr;
    
    UPROPERTY(BlueprintReadWrite, Category="Sound")
    USoundBase* ExplosionSound = nullptr;


    UPROPERTY(BlueprintReadWrite, Category="Explosion")
    bool bDelayedExplosion = false;

    UPROPERTY(BlueprintReadWrite, Category="Explosion")
    float ExplosionDelay = 2.0f;

    // ✅ 정석적인 초기화 함수 추가
    UFUNCTION(BlueprintCallable)
    void InitializeProjectile(
        float Damage,
        float AreaRadius, 
        UParticleSystem* Trail,
        UParticleSystem* Explosion,
        USoundBase* Fire,
        USoundBase* ExplosionSnd,
        bool bDelayed,
        float Delay,
        UNiagaraSystem* NiagaraTrail = nullptr,
        UNiagaraSystem* NiagaraExplosion = nullptr
    );

    // === 설정 함수들 ===
    UFUNCTION(BlueprintCallable)
    void SetProjectileAppearance(UStaticMesh* Mesh, UMaterialInterface* Material, const FVector& Scale);

    UFUNCTION(BlueprintCallable)
    void SetProjectileMovementSettings(float Speed, float GravityScale, bool bShouldBounce, float Bounciness);

    UFUNCTION(BlueprintCallable)
    void SetTrailEffectDuration(float Duration);

    void FireInDirection(const FVector& ShootDirection);

protected:
    virtual void BeginPlay() override;
    
    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
               FVector NormalImpulse, const FHitResult& Hit);
               
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                        bool bFromSweep, const FHitResult& SweepResult);

private:
    UPROPERTY()
    TSet<AActor*> HitActors;
    
    UPROPERTY()
    AActor* ProjectileShooter = nullptr;

    FTimerHandle ExplosionTimerHandle;
    FTimerHandle TrailTimerHandle;
    bool bHasExploded = false;

    // ✅ 초기화 완료 플래그
    bool bIsInitialized = false;

    UFUNCTION()
    void TriggerDelayedExplosion();

    void PlayExplosionEffects(const FVector& Location);
    void ApplyDamageToTarget(AActor* Target, const FVector& HitLocation);
    void SpawnHitEffects(const FVector& Location, const FVector& Normal); 
};
