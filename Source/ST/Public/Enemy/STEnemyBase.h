#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy/STEnemyStateComponent.h"
#include "Enemy/STPatrolPoint.h"
#include "STEnemyBase.generated.h"

UCLASS()
class ST_API ASTEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	ASTEnemyBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USTEnemyStateComponent* StateComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float PatrolSpeed = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float ChaseSpeed = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Movement")
	float InvestigationSpeed = 400.f;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="AI|Patrol")
	TArray<ASTPatrolPoint*> PatrolPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Patrol")
	int32 CurrentPatrolIndex = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Health = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Defense = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	float AttackRange = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	float ApproachDistance = 200.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WeakPoint")
	TMap<FName, float> WeakPointMultipliers;
	
	virtual auto TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	                        AController* Instigator, AActor* DamageCauser) -> float override;
	virtual void ProcessDamage(float RawDamage, FName HitBone, FVector HitLocation);

	UFUNCTION()
	virtual void Die();
	virtual void Attack() PURE_VIRTUAL(AEnemyBase::Attack, );
	virtual void SetCurrentTarget(AActor* Target) PURE_VIRTUAL(AEnemyBase::SetCurrentTarget, );
	
	UFUNCTION()
	void OnStateChanged_UpdateSpeed(EEnemyState NewState, EEnemyState PrevState);
	UFUNCTION()
	void UpdateSpeedByState(EEnemyState NewState);
	
	UFUNCTION(BlueprintPure, Category = "Enemy|Stats")
	float GetHealth() const { return Health; }
	UFUNCTION(BlueprintPure, Category = "Enemy|Stats")
	float GetMaxHealth() const { return MaxHealth; }

protected:
	virtual void BeginPlay() override;
};
