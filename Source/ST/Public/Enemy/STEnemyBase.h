#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "STEnemyBase.generated.h"

UCLASS()
class ST_API ASTEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	ASTEnemyBase();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Health = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Defense = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float AttackRange = 200.f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsDead = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeakPoint")
	TMap<FName, float> WeakPointMultipliers;

	
	virtual auto TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	                        AController* Instigator, AActor* DamageCauser) -> float override;
	virtual void ApplyDamage(float RawDamage, FName HitBone, FVector HitLocation);

	UFUNCTION(BlueprintCallable, Category = "Enemy|State")
	virtual void Die();
	virtual void Attack() PURE_VIRTUAL(AEnemyBase::Attack, );
	virtual void SetCurrentTarget(AActor* Target) PURE_VIRTUAL(AEnemyBase::SetCurrentTarget, );
	virtual bool IsAttacking() const { return false; }
};
