#include "Enemy/STEnemyMelee.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Enemy/STMeleeAnimInstance.h"
#include "Components/CapsuleComponent.h"

ASTEnemyMelee::ASTEnemyMelee()
{
	PrimaryActorTick.bCanEverTick = false;
	AttackRange = 400.f;
	bIsAttackCooldown = false;
	Defense = 20.f;
	StateComponent->SetState(EEnemyState::Idle);
}

void ASTEnemyMelee::Attack()
{
	if (StateComponent->IsInState(EEnemyState::Dead) || 
		StateComponent->IsInState(EEnemyState::Attack) || 
		bIsAttackCooldown || !
		CurrentTarget)
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
	
	StateComponent->SetState(EEnemyState::Attack);
	bIsAttackCooldown = true;
	
	GetWorldTimerManager().SetTimer(AttackCooldownHandle, [this]()
	{
		bIsAttackCooldown = false;
	}, AttackCooldown, false);
}

void ASTEnemyMelee::AttackNotify()
{
	if (AttackSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
	}
	if (SlashParticle)
	{
		UGameplayStatics::SpawnEmitterAttached(
			SlashParticle,
			GetMesh(),
			TEXT("ik_hand_r"));
	}
	if (CurrentTarget && !StateComponent->IsInState(EEnemyState::Dead))
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
				// if (GEngine && CurrentTarget)
				// {
				// 	FString Msg = FString::Printf(TEXT("Enemy Melee Hit: %s (%.0f damage)"), *CurrentTarget->GetName(), MeleeDamage);
				// 	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, Msg);
				// }
				if (CurrentTarget->ActorHasTag("Player"))
				{
					UGameplayStatics::ApplyDamage(
					CurrentTarget,
					MeleeDamage,
					GetController(),
					this,
					nullptr
				);
				}
			}

		}
	}
}

void ASTEnemyMelee::AttackEndNotify()
{
	StateComponent->SetState(EEnemyState::Idle);
}

void ASTEnemyMelee::Die()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	Super::Die();
}
