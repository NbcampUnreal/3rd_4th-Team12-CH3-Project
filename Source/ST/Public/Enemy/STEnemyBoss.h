#pragma once

#include "CoreMinimal.h"
#include "Enemy/STEnemyBase.h"
#include "Enemy/STEnemySkillComponent.h"
#include "STEnemyBoss.generated.h"

UENUM(BlueprintType)
enum class EBossPhase : uint8
{
	Phase1      UMETA(DisplayName = "Phase 1"),
	Phase2      UMETA(DisplayName = "Phase 2")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBossPhaseChanged, int32, OldPhase, int32, NewPhase);

UCLASS()
class ST_API ASTEnemyBoss : public ASTEnemyBase
{
	GENERATED_BODY()

public:
	ASTEnemyBoss();

	// 보스 페이즈
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss|Phase")
	EBossPhase CurrentPhase = EBossPhase::Phase1;

	// 스킬 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Boss|Skill")
	USTEnemySkillComponent* SkillComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss|Phase Meshes")
	TMap<EBossPhase, USkeletalMesh*> PhaseMeshes;

	// 페이즈 당 가능한 스킬
	TMap<EBossPhase, TArray<EEnemySkillID>> PhaseSkills;

	// 페이즈 변환 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnBossPhaseChanged OnPhaseChanged;

	// 현재 페이즈에서 사용 가능한 스킬들 반환
	UFUNCTION(BlueprintPure, Category="Boss")
	TArray<EEnemySkillID> GetCurrentPhaseSkills() const;
	
	virtual void Attack() override;

	UFUNCTION(BlueprintCallable, Category="Boss")
	void CheckPhase(float CurrentHealth, float Max_Health, float HealthPercentage);

	UFUNCTION(BlueprintPure, Category="Boss")
	bool IsPhaseChanging() const { return StateComponent->IsInState(EEnemyState::PhaseChanging); }
	
	UFUNCTION(BlueprintCallable, Category="Boss")
	void PhaseChangingNotify();
	
	UFUNCTION(BlueprintCallable, Category="Boss")
	void PhaseChangingEndNotify();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
							AController* EventInstigator, AActor* DamageCauser) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Boss Death")
	USoundBase* DeathSound = nullptr;
	
	virtual void BeginPlay() override;
	virtual void Die() override;

private:
	void InitializePhaseSkills();
};
