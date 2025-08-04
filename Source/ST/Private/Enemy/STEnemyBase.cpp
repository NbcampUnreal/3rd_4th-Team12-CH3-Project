#include "Enemy/STEnemyBase.h"

#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

ASTEnemyBase::ASTEnemyBase()
{
 	PrimaryActorTick.bCanEverTick = false;

	WeakPointMultipliers.Add(FName("head"), 2.0f);
	WeakPointMultipliers.Add(FName("back"), 1.5f);
}

float ASTEnemyBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if (bIsDead)
	{
		return 0.0f;
	}
	FName HitBone=NAME_None;
	FVector HitLocation=GetActorLocation();

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PDmg= static_cast<const FPointDamageEvent*>(&DamageEvent);
		HitBone=PDmg->HitInfo.BoneName;
		HitLocation=PDmg->HitInfo.ImpactPoint;
	}

	ApplyDamage(DamageAmount, HitBone, HitLocation);
	
	return DamageAmount;
}

void ASTEnemyBase::ApplyDamage(float RawDamage, FName HitBone, FVector HitLocation)
{
	if (bIsDead)
	{
		return;
	}

	float Multiplier=1.0f;
	if (WeakPointMultipliers.Contains(HitBone))
	{
		Multiplier=WeakPointMultipliers[HitBone];
	}
	float TrueDamage=FMath::Max(0.0f, RawDamage*Multiplier-Defense);
	Health=FMath::Max(0.0f, Health-TrueDamage);

	// 나중에 피격 FX나 사운드 처리

	if (Health<=0 && !bIsDead)
	{
		Die();
	}
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			6.0f,
			FColor::Green,
			*FString::Printf(TEXT("Damage: %0.f, Health: %0.f"), TrueDamage, Health)
			);
	}
}

void ASTEnemyBase::Die()
{
	if (bIsDead)
	{
		return;
	}
	bIsDead=true;

	SetLifeSpan(4.0f);
}

