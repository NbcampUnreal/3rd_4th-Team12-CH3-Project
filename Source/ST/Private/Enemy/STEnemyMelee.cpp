#include "Enemy/STEnemyMelee.h"
#include "AIController.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Enemy/STMeleeAnimInstance.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"


ASTEnemyMelee::ASTEnemyMelee()
{
	PrimaryActorTick.bCanEverTick = false;
	AttackRange = 200.f;
}

void ASTEnemyMelee::Attack()
{
	if (bIsDead || bIsAttacking || bIsAttackCooldown || !CurrentTarget)
	{
		return;
	}
	FVector MyLocation = GetActorLocation();
	FVector TargetLocation = CurrentTarget->GetActorLocation();

	FRotator LookAtRotation = (TargetLocation - MyLocation).Rotation();
	LookAtRotation.Pitch = 0.0f;
	SetActorRotation(LookAtRotation);

	float Distance = FVector::Dist(MyLocation, TargetLocation);
	if (Distance > AttackRange)
	{
		return;
	}
	
	bIsAttacking = true;
	bIsAttackCooldown = true;
	if (UAnimInstance* Anim = GetMesh()->GetAnimInstance())
	{
		USTMeleeAnimInstance* EnemyAnim = Cast<USTMeleeAnimInstance>(Anim);
		if (EnemyAnim)
		{
			EnemyAnim->bIsAttackingIn = true;
		}
	}
	
	GetWorldTimerManager().SetTimer(AttackCooldownHandle, [this]()
	{
		bIsAttackCooldown = false;
	}, AttackCooldown, false);
}

void ASTEnemyMelee::SetCurrentTarget(AActor* Target)
{
	CurrentTarget = Target;
}

void ASTEnemyMelee::AttackNotify()
{
	if (CurrentTarget && !bIsDead)
	{
		// 공격 거리 계산
		float Distance = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());
		if (Distance <= AttackRange)
		{
			FVector ToTarget = (CurrentTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			FVector Forward = GetActorForwardVector();
			float Dot = FVector::DotProduct(ToTarget, Forward);

			// 공격 좌우 판정 30도
			const float MinDot = FMath::Cos(FMath::DegreesToRadians(30.f));

			if (Dot >= MinDot)
			{
				UGameplayStatics::ApplyPointDamage(
				CurrentTarget,
				MeleeDamage,
				Forward,
				FHitResult(),
				GetController(),
				this,
				nullptr
				);
			}
		}
	}
}

void ASTEnemyMelee::AttackEndNotify()
{
	bIsAttacking = false;
}

void ASTEnemyMelee::BeginPlay()
{
	Super::BeginPlay();
}

void ASTEnemyMelee::Die()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);
	bIsAttacking = false;

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
		USTMeleeAnimInstance* EnemyAnim = Cast<USTMeleeAnimInstance>(Anim);
		if (EnemyAnim)
		{
			EnemyAnim->bIsDead = true;
		}
	}
	
	Super::Die();
}
