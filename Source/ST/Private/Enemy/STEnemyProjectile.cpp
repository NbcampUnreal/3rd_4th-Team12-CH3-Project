#include "Enemy/STEnemyProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"


ASTEnemyProjectile::ASTEnemyProjectile()
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
	CollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
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

    InitialLifeSpan = 3.0f;
}

void ASTEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (ProjectileParticle && ProjectileParticleComp)
	{
		ProjectileParticleComp->SetTemplate(ProjectileParticle);
		ProjectileParticleComp->ActivateSystem();
	}
	
	if (CollisionComponent)
	{
		if (GetInstigator())
		{
			CollisionComponent->IgnoreActorWhenMoving(GetInstigator(), true);
		}
		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ASTEnemyProjectile::OnOverlapBegin);
		CollisionComponent->OnComponentHit.AddDynamic(this,&ASTEnemyProjectile::OnHit);
	}
}

void ASTEnemyProjectile::FireInDirection(const FVector& ShootDirection)
{
	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity=ShootDirection*ProjectileMovement->InitialSpeed;
	}
}

void ASTEnemyProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	Destroy();
}

void ASTEnemyProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// if (OtherActor)
	// {
	// 	if (GEngine)
	// 	{
	// 		FString DebugMsg = FString::Printf(TEXT("Projectile Overlap: %s"), *OtherActor->GetName());
	// 		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, DebugMsg);
	// 	}
	// }
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		
		UGameplayStatics::ApplyDamage(
			OtherActor,
			ProjectileDamage, 
			GetInstigator() ? GetInstigator()->GetController() : nullptr,
			this,
			nullptr
		);
	}
	
	Destroy();
}