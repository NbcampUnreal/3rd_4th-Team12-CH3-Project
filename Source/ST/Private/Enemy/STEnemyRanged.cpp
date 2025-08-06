#include "Enemy/STEnemyRanged.h"
#include "Enemy/STEnemyProjectile.h"
#include "AIController.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "Enemy/STRangedAnimInstance.h"
#include "Kismet/GameplayStatics.h"

ASTEnemyRanged::ASTEnemyRanged()
{
	PrimaryActorTick.bCanEverTick = false;
	AttackRange = 700.f;
	AmmoCount=MaxAmmo;
	bIsAttackCooldown = false;
	StateComponent->SetState(EEnemyState::Idle);
}

void ASTEnemyRanged::BeginPlay()
{
	Super::BeginPlay();
}

void ASTEnemyRanged::Attack()
{
	if (StateComponent->IsInState(EEnemyState::Dead) ||
		StateComponent->IsInState(EEnemyState::Attack) ||
		StateComponent->IsInState(EEnemyState::Reload) ||
		bIsAttackCooldown ||
		!CurrentTarget)
	{
		return;
	}
	if (AmmoCount <= 0)
	{
		Reload();
		return;
	}
	
	StateComponent->SetState(EEnemyState::Attack);
	bIsAttackCooldown = true;

	FVector MyLocation = GetActorLocation();
	FVector TargetLocation = CurrentTarget->GetActorLocation();
	FRotator LookAtRotation = (TargetLocation - MyLocation).Rotation();

	// 수평고정
	LookAtRotation.Pitch = 0.f;
	SetActorRotation(LookAtRotation);
	
	GetWorldTimerManager().SetTimer(AttackCooldownHandle, [this]()
		{
			bIsAttackCooldown = false;
		}, FireRate, false);
}

void ASTEnemyRanged::FireProjectile()
{
	if (!ProjectileClass)
	{
		return;
	}
	
	FVector SpawnLocation = GetMesh()->GetSocketLocation(TEXT("gun_barrel"));
	FVector TargetLocation = CurrentTarget->GetActorLocation();
	FVector FireDirection = (TargetLocation - SpawnLocation).GetSafeNormal();
	FRotator FireRotation = FireDirection.Rotation();

	float MuzzleOffset = 20.0f;
	SpawnLocation += FireDirection * MuzzleOffset;

	ASTEnemyProjectile* Projectile = GetWorld()->SpawnActor<ASTEnemyProjectile>(ProjectileClass, SpawnLocation, FireRotation);
	if (Projectile)
	{
		Projectile->SetInstigator(this);
	}
	
}

void ASTEnemyRanged::SetCurrentTarget(AActor* Target)
{
	CurrentTarget = Target;
}

void ASTEnemyRanged::AttackNotify()
{
	FVector MyLocation = GetActorLocation();
	FVector TargetLocation = CurrentTarget->GetActorLocation();
	FRotator LookAtRotation = (TargetLocation - MyLocation).Rotation();

	// 수평고정
	LookAtRotation.Pitch = 0.f;
	SetActorRotation(LookAtRotation);
	
	if (AmmoCount > 0 && CurrentTarget && !StateComponent->IsInState(EEnemyState::Dead))
	{
		FireProjectile();
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}
		if (AttackParticle)
		{
			UGameplayStatics::SpawnEmitterAttached(
			AttackParticle,
			GetMesh(),
			TEXT("gun_barrel"),
			FVector::ZeroVector,
			FRotator(0.f, 90.f, 0.f),
			EAttachLocation::KeepRelativeOffset,
			true);
		}
		AmmoCount--;
	}
}

void ASTEnemyRanged::AttackEndNotify()
{
	StateComponent->SetState(EEnemyState::Idle);
}

void ASTEnemyRanged::Reload()
{
	if (StateComponent->IsInState(EEnemyState::Dead) ||
		StateComponent->IsInState(EEnemyState::Reload) ||
		AmmoCount == MaxAmmo)
	{
		return;
	}
	StateComponent->SetState(EEnemyState::Reload);
	
	GetWorldTimerManager().SetTimer(ReloadHandle, this, &ASTEnemyRanged::FinishReload, ReloadTime, false);
}

void ASTEnemyRanged::FinishReload()
{
	AmmoCount=MaxAmmo;
	StateComponent->SetState(EEnemyState::Idle);
}

void ASTEnemyRanged::Die()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);
	bIsAttackCooldown = false;

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
		if (AIController->BrainComponent)
		{
			AIController->BrainComponent->StopLogic(TEXT("Dead"));
		}
	}
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	Super::Die();
}




