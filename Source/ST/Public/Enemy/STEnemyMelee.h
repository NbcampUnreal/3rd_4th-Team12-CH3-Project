#pragma once

#include "CoreMinimal.h"
#include "Enemy/STEnemyBase.h"
#include "STEnemyMelee.generated.h"

UCLASS()
class ST_API ASTEnemyMelee : public ASTEnemyBase
{
	GENERATED_BODY()

public:
	ASTEnemyMelee();
	
	virtual void Attack() override;

	UFUNCTION(BlueprintCallable)
	void AttackNotify();
	UFUNCTION(BlueprintCallable)
	void AttackEndNotify();

protected:
	FTimerHandle AttackCooldownHandle;

	bool bIsAttackCooldown = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float AttackCooldown = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float MeleeDamage = 10.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	USoundBase* AttackSound;
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	UParticleSystem* SlashParticle;
	
	virtual void Die() override;
};
