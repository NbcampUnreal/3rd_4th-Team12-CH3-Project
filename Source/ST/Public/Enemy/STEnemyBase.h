#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy/STEnemyStateComponent.h"
#include "Enemy/STPatrolPoint.h"
#include "STEnemyBase.generated.h"

// SH: UI로 데미지 표시를 보내기 위한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDamageTaken, AActor*, DamagedActor, float, DamageAmount, bool, bCritical);
// SH: UI로 적 사망상태를 보내기 위한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDied, AActor*, DeadEnemy);
// 적의 체력이 변화할 때 보내기 위한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnEnemyHealthChanged, float, CurrentHealth, float, MaxHealth, float, HealthPercentage);

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
	float ChaseSpeed = 800.f;
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

	// 현재 타켓
	UPROPERTY(BlueprintReadOnly, Category = "AI|Target")
	AActor* CurrentTarget = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Death")
	UAnimMontage* DeathMontage;
	UPROPERTY(EditDefaultsOnly, Category = "SFX")
	USoundBase* DeathSound;

	// 적 체력 변화 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnEnemyHealthChanged OnEnemyHealthChanged;
	// SH: UI로 데미지 표시를 보내기 위한 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnDamageTaken OnDamageTaken;
	// SH: UI로 적 사망상태를 보내기 위한 델리게이트
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnDied OnDied;

	virtual void SetCurrentTarget(AActor* Target) { CurrentTarget = Target; }
	UFUNCTION(BlueprintPure, Category = "AI|Target")
	AActor* GetCurrentTarget() const { return CurrentTarget; }
	
	virtual auto TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	                        AController* Instigator, AActor* DamageCauser) -> float override;
	virtual void ProcessDamage(float RawDamage, FName HitBone, FVector HitLocation);

	UFUNCTION()
	virtual void Die();
	virtual void Attack() PURE_VIRTUAL(AEnemyBase::Attack, );
	
	UFUNCTION()
	void OnStateChanged_UpdateSpeed(EEnemyState NewState, EEnemyState PrevState);
	UFUNCTION()
	void UpdateSpeedByState(EEnemyState NewState);
	
	UFUNCTION(BlueprintPure, Category = "Enemy|Stats")
	float GetHealth() const { return Health; }
	UFUNCTION(BlueprintPure, Category = "Enemy|Stats")
	float GetMaxHealth() const { return MaxHealth; }
	UFUNCTION(BlueprintPure, Category = "Enemy|Stats")
	float GetHealthPercentage() const { return MaxHealth > 0.f ? (Health / MaxHealth) * 100.f : 0.f; }
	
protected:
	float CachedDamageReductionMultiplier = 1.0f;
	
	virtual void BeginPlay() override;
	
	// 방어력 배율 계산 및 캐싱
	void UpdateDamageReductionMultiplier();
private:
	// 체력 변화 알림 함수
	void NotifyHealthChanged();
};
