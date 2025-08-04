#pragma once

#include "CoreMinimal.h"
#include "Enemy/STEnemyBase.h"
#include "STEnemyRanged.generated.h"

class ASTEnemyProjectile;

UCLASS()
class ST_API ASTEnemyRanged : public ASTEnemyBase
{
	GENERATED_BODY()

public:
	ASTEnemyRanged();

	UPROPERTY(BlueprintReadWrite, Category="Anim")
	bool bIsAttacking = false;
	UPROPERTY(BlueprintReadWrite, Category="Anim")
	bool bIsReloading = false;
	UPROPERTY(BlueprintReadWrite, Category="Anim")
	bool bIsAttackCooldown = false;

	virtual void Attack() override;
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void FireProjectile();
	virtual void SetCurrentTarget(AActor* Target) override;
	UFUNCTION(BlueprintCallable)
	void AttackNotify();
	UFUNCTION(BlueprintCallable)
	void AttackEndNotify();

protected:
	FTimerHandle AttackCooldownHandle;
	FTimerHandle ReloadHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 MaxAmmo = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 AmmoCount = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float FireRate = 1.0f; // 초당 발사 간격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float ReloadTime = 2.0f; // 리로드 소요 시간(초)
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<ASTEnemyProjectile> ProjectileClass;
	UPROPERTY()
	AActor* CurrentTarget = nullptr;

	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Reload();
	UFUNCTION()
	void FinishReload();
	virtual void Die() override;
	virtual bool IsAttacking() const override { return bIsAttacking; }
};
