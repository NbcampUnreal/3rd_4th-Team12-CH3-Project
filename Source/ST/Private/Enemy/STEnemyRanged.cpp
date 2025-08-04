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
	bIsAttacking = false;
	bIsAttackCooldown = false;
	bIsReloading = false;
}

void ASTEnemyRanged::BeginPlay()
{
	Super::BeginPlay();
}

void ASTEnemyRanged::Attack()
{
	if (bIsDead || bIsAttacking || bIsAttackCooldown || bIsReloading || !CurrentTarget)
	{
		return;
	}
	
	bIsAttacking = true;
	bIsAttackCooldown = true;
	
	FVector MyLocation = GetActorLocation();
	FVector TargetLocation = CurrentTarget->GetActorLocation();
	FRotator LookAtRotation = (TargetLocation - MyLocation).Rotation();

	// 수평고정
	LookAtRotation.Pitch = 0.f;
	SetActorRotation(LookAtRotation);

	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
	{
		USTRangedAnimInstance* EnemyAnim = Cast<USTRangedAnimInstance>(Anim);
		if (EnemyAnim)
		{
			EnemyAnim->bIsAttackingIn = true;
		}
	}
	
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
	
	FVector SpawnLocation = GetMesh()->GetSocketLocation(TEXT("hand_r"));
	FVector TargetLocation = CurrentTarget->GetActorLocation();
	FVector FireDirection = (TargetLocation - SpawnLocation).GetSafeNormal();
	FRotator FireRotation = FireDirection.Rotation();

	float MuzzleOffset = 80.0f;
	SpawnLocation += FireDirection * MuzzleOffset;

	ASTEnemyProjectile* Projectile = GetWorld()->SpawnActor<ASTEnemyProjectile>(ProjectileClass, SpawnLocation, FireRotation);
	if (Projectile)
	{
		Projectile->SetInstigator(this);
	}
	
	// 나중에 이펙트, 사운드 추가 해야함
	
}

void ASTEnemyRanged::SetCurrentTarget(AActor* Target)
{
	CurrentTarget = Target;
}

void ASTEnemyRanged::AttackNotify()
{
	if (AmmoCount > 0 && CurrentTarget && !bIsDead)
	{
		FireProjectile();
		AmmoCount--;
	}
	else if (AmmoCount <= 0 && !bIsDead)
	{
		Reload();
	}
}

void ASTEnemyRanged::AttackEndNotify()
{
	bIsAttacking = false;
	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
	{
		USTRangedAnimInstance* EnemyAnim = Cast<USTRangedAnimInstance>(Anim);
		if (EnemyAnim)
		{
			EnemyAnim->bIsAttackingIn = false;
		}
	}
}

void ASTEnemyRanged::Reload()
{
	if (bIsDead || bIsReloading || AmmoCount == MaxAmmo)
	{
		return;
	}
	bIsReloading = true;
	
	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
	{
		USTRangedAnimInstance* EnemyAnim = Cast<USTRangedAnimInstance>(Anim);
		if (EnemyAnim)
		{
			EnemyAnim->bIsReloading = true;
		}
	}
	
	GetWorldTimerManager().SetTimer(ReloadHandle, this, &ASTEnemyRanged::FinishReload, ReloadTime, false);

	// 나중에 재장전 사운드 구현 해야함
}

void ASTEnemyRanged::FinishReload()
{
	AmmoCount=MaxAmmo;
	bIsReloading = false;
	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
	{
		USTRangedAnimInstance* EnemyAnim = Cast<USTRangedAnimInstance>(Anim);
		if (EnemyAnim)
		{
			EnemyAnim->bIsReloading = false;
		}
	}
}

void ASTEnemyRanged::Die()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);
	bIsReloading = false;
	bIsAttacking = false;
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
	
	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
	{
		USTRangedAnimInstance* EnemyAnim = Cast<USTRangedAnimInstance>(Anim);
		if (EnemyAnim)
		{
			EnemyAnim->bIsDead = true;
		}
	}

	Super::Die();
}




