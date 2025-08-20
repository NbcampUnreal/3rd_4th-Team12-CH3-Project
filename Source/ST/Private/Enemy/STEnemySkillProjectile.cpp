#include "Enemy/STEnemySkillProjectile.h"
#include "NiagaraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

ASTEnemySkillProjectile::ASTEnemySkillProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    CollisionComponent->InitSphereRadius(8.0f);
    CollisionComponent->SetCollisionProfileName(TEXT("Custom"));
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    CollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
    CollisionComponent->SetGenerateOverlapEvents(true);
    RootComponent = CollisionComponent;

    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
    ProjectileMesh->SetupAttachment(RootComponent);
    ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->SetUpdatedComponent(CollisionComponent);
    ProjectileMovement->InitialSpeed = 2000.f;
    ProjectileMovement->MaxSpeed = 2000.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->ProjectileGravityScale = 0.5f;

    ProjectileParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ProjectileParticleComp"));
    ProjectileParticleComp->SetupAttachment(RootComponent);
    ProjectileParticleComp->bAutoActivate = false;

    ProjectileNiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileNiagaraComp"));
    ProjectileNiagaraComp->SetupAttachment(RootComponent);
    ProjectileNiagaraComp->SetAutoActivate(false);

    InitialLifeSpan = 0.0f;
}

void ASTEnemySkillProjectile::InitializeProjectile(
    float Damage,
    float AreaRadius, 
    UParticleSystem* Trail,
    UParticleSystem* Explosion,
    USoundBase* Fire,
    USoundBase* ExplosionSnd,
    bool bDelayed,
    float Delay,
    UNiagaraSystem* NiagaraTrail,
    UNiagaraSystem* NiagaraExplosion)
{
    ProjectileDamage = Damage;
    AreaDamageRadius = AreaRadius;
    TrailEffect = Trail;
    ExplosionEffect = Explosion;
    NiagaraTrailEffect = NiagaraTrail;
    NiagaraExplosionEffect = NiagaraExplosion;
    FireSound = Fire;
    ExplosionSound = ExplosionSnd;
    bDelayedExplosion = bDelayed;
    ExplosionDelay = Delay;
    ProjectileShooter = GetInstigator();
    
    bIsInitialized = true;
    
    if (NiagaraTrailEffect && ProjectileNiagaraComp)
    {
        ProjectileNiagaraComp->SetAsset(NiagaraTrailEffect);
        ProjectileNiagaraComp->Activate();
    }
    else if (TrailEffect && ProjectileParticleComp)
    {
        ProjectileParticleComp->SetTemplate(TrailEffect);
        ProjectileParticleComp->ActivateSystem();
    }

    if (FireSound)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, GetActorLocation());
    }

    // 지연 폭발 설정
    if (bDelayedExplosion && ExplosionDelay > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(
            ExplosionTimerHandle,
            this,
            &ASTEnemySkillProjectile::TriggerDelayedExplosion,
            ExplosionDelay,
            false
        );
    }
    
}

void ASTEnemySkillProjectile::BeginPlay()
{
    Super::BeginPlay();

    if (bIsInitialized)
    {
        // 충돌 설정
        if (CollisionComponent)
        {
            if (GetInstigator())
            {
                CollisionComponent->IgnoreActorWhenMoving(GetInstigator(), true);
            }
            CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASTEnemySkillProjectile::OnOverlapBegin);
            CollisionComponent->OnComponentHit.AddDynamic(this, &ASTEnemySkillProjectile::OnHit);
        }
    }
}

void ASTEnemySkillProjectile::FireInDirection(const FVector& ShootDirection)
{
    if (ProjectileMovement)
    {
        ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
    }
}

void ASTEnemySkillProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    FVector NormalImpulse, const FHitResult& Hit)
{
    if (!bDelayedExplosion)
    {
        PlayExplosionEffects(Hit.Location);
        
        FTimerHandle DestroyTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            DestroyTimerHandle,
            [this]()
            {
                Destroy();
            },
            0.1f,
            false
        );
    }
}

void ASTEnemySkillProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || !OtherActor->ActorHasTag("Player")) return;
    if (HitActors.Contains(OtherActor)) return;

    FVector HitLocation = SweepResult.bBlockingHit ? FVector(SweepResult.Location) : OtherActor->GetActorLocation();
    
    // 지연 폭발이 아니면 즉시 폭발
    if (!bDelayedExplosion)
    {
        ApplyDamageToTarget(OtherActor, HitLocation);
        
        PlayExplosionEffects(HitLocation);
        
        FTimerHandle DestroyTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            DestroyTimerHandle,
            [this]()
            {
                Destroy();
            },
            0.1f,
            false
        );
    }
}

void ASTEnemySkillProjectile::TriggerDelayedExplosion()
{
    if (bHasExploded) return;
    
    bHasExploded = true;
    const FVector ExplosionLocation = GetActorLocation();

    // 폭발 데미지
    if (AreaDamageRadius > 0.f)
    {
        TArray<AActor*> OverlappingActors;
        UKismetSystemLibrary::SphereOverlapActors(
            GetWorld(),
            ExplosionLocation,
            AreaDamageRadius,
            TArray<TEnumAsByte<EObjectTypeQuery>>(),
            APawn::StaticClass(),
            TArray<AActor*>{ProjectileShooter},
            OverlappingActors
        );
        
        for (AActor* Actor : OverlappingActors)
        {
        	// Player 태그만 데미지 적용
            if (Actor && Actor->ActorHasTag("Player"))
            {
                UGameplayStatics::ApplyDamage(
                    Actor,
                    ProjectileDamage,
                    GetInstigator() ? GetInstigator()->GetController() : nullptr,
                    this,
                    nullptr
                );
            }
        }
    }

    PlayExplosionEffects(ExplosionLocation);
    FTimerHandle DestroyTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(
        DestroyTimerHandle,
        [this]()
        {
            Destroy();
        },
        0.1f,
        false
    );
}

void ASTEnemySkillProjectile::PlayExplosionEffects(const FVector& Location)
{
    // 폭발 파티클
    if (NiagaraExplosionEffect)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            GetWorld(), 
            NiagaraExplosionEffect,
            Location,
            FRotator::ZeroRotator,
            FVector::OneVector,
            true
        );
    }
    else if (ExplosionEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(), 
            ExplosionEffect, 
            Location,
            FRotator::ZeroRotator,
            FVector::OneVector,
            true  // bAutoDestroy = true
        );
    }

    // 폭발 사운드
    if (ExplosionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, Location);
    }
}

void ASTEnemySkillProjectile::ApplyDamageToTarget(AActor* Target, const FVector& HitLocation)
{
    if (!Target) return;

    if (!Target->ActorHasTag("Player"))
    {
        return;
    }
    
    if (AreaDamageRadius > 0.f)
    {
        // 범위 데미지
        TArray<AActor*> IgnoreActors;
        if (ProjectileShooter) IgnoreActors.Add(ProjectileShooter);
        
        UGameplayStatics::ApplyRadialDamage(
            GetWorld(),
            ProjectileDamage,
            HitLocation,
            AreaDamageRadius,
            nullptr,
            IgnoreActors,
            this,
            GetInstigator() ? GetInstigator()->GetController() : nullptr,
            true
        );
    }
    else
    {
        // 단일 타겟 데미지
        UGameplayStatics::ApplyDamage(
            Target,
            ProjectileDamage,
            GetInstigator() ? GetInstigator()->GetController() : nullptr,
            this,
            nullptr
        );
    }
}

void ASTEnemySkillProjectile::SpawnHitEffects(const FVector& Location, const FVector& Normal)
{
    if (ExplosionEffect)
    {
        const FRotator HitRotation = Normal.Rotation();
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, Location, HitRotation);
    }
    
    if (ExplosionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, Location);
    }
}

void ASTEnemySkillProjectile::SetProjectileAppearance(UStaticMesh* Mesh, UMaterialInterface* Material, const FVector& Scale)
{
    if (!ProjectileMesh) return;

    if (Mesh)
    {
        ProjectileMesh->SetStaticMesh(Mesh);
    }
	
    if (Material)
    {
        ProjectileMesh->SetMaterial(0, Material);
    }

    if (!Scale.IsNearlyZero())
    {
        ProjectileMesh->SetRelativeScale3D(Scale);
    }
}

void ASTEnemySkillProjectile::SetProjectileMovementSettings(float Speed, float GravityScale, bool bShouldBounce, float Bounciness)
{
    if (!ProjectileMovement) return;

    ProjectileMovement->InitialSpeed = Speed;
    ProjectileMovement->MaxSpeed = Speed;
    ProjectileMovement->ProjectileGravityScale = GravityScale;
    ProjectileMovement->bShouldBounce = bShouldBounce;
    ProjectileMovement->Bounciness = Bounciness;
}

void ASTEnemySkillProjectile::SetTrailEffectDuration(float Duration)
{
    if (!ProjectileParticleComp || Duration <= 0.0f) return;

    GetWorld()->GetTimerManager().SetTimer(
        TrailTimerHandle,
        [this]()
        {
            if (ProjectileParticleComp && IsValid(ProjectileParticleComp))
            {
                ProjectileParticleComp->DeactivateSystem();
            }
        },
        Duration,
        false
    );
}
