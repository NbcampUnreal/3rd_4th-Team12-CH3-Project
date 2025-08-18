#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "Animation/AnimMontage.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "NiagaraSystem.h"
#include "STEnemySkillTypes.generated.h"

UENUM(BlueprintType)
enum class EEnemySkillID : uint8
{
    None                UMETA(DisplayName = "None"),
    Melee               UMETA(DisplayName = "Melee"),
    BossMelee1          UMETA(DisplayName = "BossMelee1"),
    BossMelee2          UMETA(DisplayName = "BossMelee2"),
    BossMelee3          UMETA(DisplayName = "BossMelee3"),
    BossBomb            UMETA(DisplayName = "BossBomb"),
    BossMeleeExplosion  UMETA(DisplayName = "BossMeleeExplosion"),
    BossFireAttack      UMETA(DisplayName = "BossFireAttack"),
    BossHook            UMETA(DisplayName = "BossHook"),
    BossHookUltimate    UMETA(DisplayName = "BossHookUltimate"),
    BossDashUltimate    UMETA(DisplayName = "BossDashUltimate"),
    BossUpperUltimate   UMETA(DisplayName = "BossUpperUltimate")
};

UENUM(BlueprintType)
enum class ESkillActionType : uint8
{
    PlayMontage         UMETA(DisplayName = "Play Montage"),
    SpawnProjectile     UMETA(DisplayName = "Spawn Projectile"),
    ExecuteDash         UMETA(DisplayName = "Execute Dash"),
    SetVisibility         UMETA(DisplayName = "Set Visibility")
};

// 다중 파티클 설정 구조체
USTRUCT(BlueprintType)
struct FParticleEffectSetting
{
    GENERATED_BODY()

    // 파티클 시스템
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Particle")
    UParticleSystem* ParticleSystem = nullptr;

    // 나이아가라 시스템
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Particle")
    UNiagaraSystem* NiagaraSystem = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Particle")
    TArray<FName> SocketNames;

    // 파티클 지속 시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Particle")
    float Duration = 0.0f;

    // 파티클 크기 조절
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Particle")
    FVector Scale = FVector(1.0f, 1.0f, 1.0f);

    // 소켓 기준 오프셋
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Particle")
    FVector LocationOffset = FVector::ZeroVector;

    // 회전 오프셋
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Particle")
    FRotator RotationOffset = FRotator::ZeroRotator;

    // 자동 삭제 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Particle")
    bool bAutoDestroy = true;

    FParticleEffectSetting()
    {
        SocketNames.Add(TEXT("None"));
    }
};

USTRUCT(BlueprintType)
struct FSkillAction
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action")
    ESkillActionType ActionType = ESkillActionType::PlayMontage;

    // === 애니메이션 설정 ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
    UAnimMontage* AnimMontage = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
    float PlayRate = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
    float BlendInTime = 0.25f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
    float BlendOutTime = 0.25f;

    // 다중 파티클 시스템
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Effects", meta=(TitleProperty="ParticleSystem"))
    TArray<FParticleEffectSetting> ParticleEffects;

    // 사운드 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
    USoundBase* SoundEffect = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
    TArray<FName> SoundSocketNames;

    // 투사체 기본 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile", meta=(EditCondition="ActionType==ESkillActionType::SpawnProjectile"))
    TSubclassOf<AActor> ProjectileClass = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile", meta=(EditCondition="ActionType==ESkillActionType::SpawnProjectile"))
    FName SpawnSocketName = TEXT("weapon_r");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile", meta=(EditCondition="ActionType==ESkillActionType::SpawnProjectile"))
    float ProjectileDamage = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile", meta=(EditCondition="ActionType==ESkillActionType::SpawnProjectile"))
    float DamageRadius = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Appearance", meta=(EditCondition="ActionType==ESkillActionType::SpawnProjectile"))
    UStaticMesh* ProjectileMesh = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Appearance", meta=(EditCondition="ActionType==ESkillActionType::SpawnProjectile"))
    UMaterialInterface* ProjectileMaterial = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Appearance", meta=(EditCondition="ActionType==ESkillActionType::SpawnProjectile"))
    FVector ProjectileMeshScale = FVector(1.0f, 1.0f, 1.0f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Physics", meta=(EditCondition="ActionType==ESkillActionType::SpawnProjectile"))
    float ProjectileSpeed = 2000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Physics", meta=(EditCondition="ActionType==ESkillActionType::SpawnProjectile"))
    float ProjectileGravityScale = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Physics", meta=(EditCondition="ActionType==ESkillActionType::SpawnProjectile"))
    bool bProjectileShouldBounce = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Physics", meta=(EditCondition="ActionType==ESkillActionType::SpawnProjectile"))
    float ProjectileBounciness = 0.3f;

    // 프로젝타일 이펙트
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Effects", meta=(EditCondition="ActionType==ESkillActionType::SpawnProjectile"))
    UParticleSystem* ProjectileTrailEffect = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Effects", meta=(EditCondition="ActionType==ESkillActionType::SpawnProjectile"))
    UNiagaraSystem* ProjectileNiagaraTrailEffect = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Effects", meta=(EditCondition="ActionType==ESkillActionType::SpawnProjectile"))
    float ProjectileTrailDuration = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Effects", meta=(EditCondition="ActionType==ESkillActionType::SpawnProjectile"))
    USoundBase* ProjectileFireSound = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Effects", meta=(EditCondition="ActionType==ESkillActionType::SpawnProjectile"))
    USoundBase* ProjectileFlightSound = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Effects", meta=(EditCondition="ActionType==ESkillActionType::SpawnProjectile"))
    UParticleSystem* ProjectileHitEffect = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Effects", meta=(EditCondition="ActionType==ESkillActionType::SpawnProjectile"))
    UNiagaraSystem* ProjectileNiagaraHitEffect = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Effects", meta=(EditCondition="ActionType==ESkillActionType::SpawnProjectile"))
    USoundBase* ProjectileHitSound = nullptr;

    // 프로젝타일 폭발 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Explosion", meta=(EditCondition="ActionType==ESkillActionType::SpawnProjectile"))
    bool bDelayedExplosion = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile Explosion", meta=(EditCondition="ActionType==ESkillActionType::SpawnProjectile && bDelayedExplosion"))
    float ExplosionDelay = 2.0f;

    // 대시 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dash", meta=(EditCondition="ActionType==ESkillActionType::ExecuteDash"))
    float DashDistance = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dash", meta=(EditCondition="ActionType==ESkillActionType::ExecuteDash"))
    float DashSpeed = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dash", meta=(EditCondition="ActionType==ESkillActionType::ExecuteDash"))
    bool bDashToTarget = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dash", meta=(EditCondition="ActionType==ESkillActionType::ExecuteDash"))
    UAnimMontage* DashLoopMontage = nullptr;

    // 실행 타이밍
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Timing")
    float ExecutionDelay = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Timing")
    bool bWaitForPreviousComplete = true;

    // Set Visibility
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visibility", meta=(EditCondition="ActionType==ESkillActionType::SetVisibility"))
    bool bVisible = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Visibility", meta=(EditCondition="ActionType==ESkillActionType::SetVisibility"))
    float VisibilityDuration = 0.0f;


    FSkillAction()
    {
        SoundSocketNames.Add(TEXT("None"));
    }
};

// 스킬 데이터 구조체
USTRUCT(BlueprintType)
struct FEnemySkillData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Basic")
    EEnemySkillID SkillID = EEnemySkillID::Melee;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Basic")
    float Range = 300.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Basic")
    float Cooldown = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Actions", meta=(TitleProperty="ActionType"))
    TArray<FSkillAction> Actions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Legacy Damage", meta=(ToolTip="Used by AttackNotify in montages"))
    float Damage = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Legacy Damage", meta=(ToolTip="Used by AttackNotify in montages, 0 = single target"))
    float AreaRadius = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Legacy Sound")
    USoundBase* CastSound = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Legacy Sound")
    TArray<FName> CastSoundSockets;

    FEnemySkillData()
    {
        Actions.AddDefaulted();
        CastSoundSockets.Add(TEXT("None"));
    }
};
