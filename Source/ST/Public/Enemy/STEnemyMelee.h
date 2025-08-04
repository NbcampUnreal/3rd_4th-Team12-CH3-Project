#pragma once

#include "CoreMinimal.h"
#include "Enemy/STEnemyBase.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "STEnemyMelee.generated.h"

UCLASS()
class ST_API ASTEnemyMelee : public ASTEnemyBase
{
	GENERATED_BODY()

public:
	ASTEnemyMelee();

	UPROPERTY(BlueprintReadWrite, Category="Anim")
	bool bIsAttacking = false;
	
	virtual void Attack() override;
	virtual void SetCurrentTarget(AActor* Target) override;

	UFUNCTION(BlueprintCallable)
	void AttackNotify();
	UFUNCTION(BlueprintCallable)
	void AttackEndNotify();

protected:
	FTimerHandle AttackCooldownHandle;
	
	UPROPERTY()
	AActor* CurrentTarget=nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float AttackCooldown = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float MeleeDamage = 20.f;
	bool bIsAttackCooldown = false;
	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	USoundBase* AttackSound;
	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	USoundBase* DeathSound;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UParticleSystem* SlashParticle;
	
	virtual void BeginPlay() override;
	virtual void Die() override;
	virtual bool IsAttacking() const override { return bIsAttacking; }
};
