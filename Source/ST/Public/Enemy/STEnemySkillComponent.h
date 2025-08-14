#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "STEnemySkillTypes.h"
#include "STEnemySkillComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ST_API USTEnemySkillComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    USTEnemySkillComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Skill")
    UDataTable* SkillDataTable = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Skill")
    TMap<EEnemySkillID, float> SkillLastUsedTime;

    // 스킬 실행 함수
    UFUNCTION(BlueprintCallable, Category="Skill")
    bool TryActivateSkill(EEnemySkillID SkillID, AActor* Target);

    UFUNCTION(BlueprintPure, Category="Skill")
    bool IsSkillReady(EEnemySkillID SkillID) const;

    UFUNCTION(BlueprintPure, Category="Skill")
    bool CanExecuteAnySkill() const;

    const FEnemySkillData* GetSkillData(EEnemySkillID SkillID) const;

    // 스킬이 실행되면 데이터 테이블에서 가져온 스킬 검사 후 정해진 Action들을 수행
    UFUNCTION(BlueprintCallable, Category="Action")
    void ExecuteSkillActions(const FEnemySkillData& SkillData, AActor* Target);

    // 하나의 스킬에 여러 Action들이 있고 그 중 다음 Action 실행
    UFUNCTION(BlueprintCallable, Category="Action")
    void ExecuteNextAction();

    // 노티파이 함수들
    UFUNCTION(BlueprintCallable, Category="Notify")
    void AttackNotify();

    UFUNCTION(BlueprintCallable, Category="Notify")
    void AttackEndNotify();

    UFUNCTION(BlueprintCallable, Category="Notify")
    void LaunchProjectile();

    // 파티클 및 이펙트
    UFUNCTION(BlueprintCallable, Category="Skill")
    void PlayMultipleParticleEffects(const TArray<FParticleEffectSetting>& ParticleEffects);

    UFUNCTION(BlueprintCallable, Category="Skill")
    void PlaySingleParticleEffect(const FParticleEffectSetting& ParticleEffect);

    UFUNCTION(BlueprintCallable, Category="Skill")
    void CleanupActiveParticles();

    // 실제 데미지 적용
    UFUNCTION(BlueprintCallable, Category="Skill") 
    void ExecuteSkill(const FEnemySkillData& Skill, AActor* Target);

    // 캐시된 스킬 데이터 접근
    UFUNCTION(BlueprintPure, Category="Skill")
    const TArray<EEnemySkillID>& GetSkillsByPriority() const { return SkillsByPriority; }
    const FEnemySkillData* GetCachedSkillData(EEnemySkillID SkillID) const;

	UFUNCTION(BlueprintCallable, Category="Skill")
	void ForceStopAllSkills();

protected:
    UPROPERTY()
    class ASTEnemyBase* OwnerEnemy = nullptr;

    UPROPERTY()
    EEnemySkillID CurrentSkillID = EEnemySkillID::Melee;

    UPROPERTY()
    EEnemySkillID LastActivatedSkillID = EEnemySkillID::Melee;

    UPROPERTY()
    AActor* CurrentTarget = nullptr;

    UPROPERTY()
    TArray<FSkillAction> CurrentActionSequence;

    UPROPERTY()
    int32 CurrentActionIndex = -1;

    UPROPERTY()
    bool bExecutingActionSequence = false;

    FTimerHandle ActionTimerHandle;

    UPROPERTY()
    TArray<USceneComponent*> ActiveParticleComponents;

    UPROPERTY()
    bool bIsAnySkillExecuting = false;

    UPROPERTY()
    float LastSkillExecutionTime = 0.0f;

    UPROPERTY()
    float MinSkillInterval = 1.0f; // 스킬 간 최소 간격 (1초)

	virtual void BeginPlay() override;

private:
    UPROPERTY()
    TArray<EEnemySkillID> SkillsByPriority; // 쿨타임 긴 순서대로 정렬된 모든 스킬
    
    TMap<EEnemySkillID, const FEnemySkillData*> CachedSkillData;
    
    UPROPERTY()
    FVector OriginalPosition;
    
    UPROPERTY()
    bool bPositionModified = false;

	UPROPERTY()
	bool bForceStoppedByPhaseChange = false;


    void InitializeSkillCache();

    // Action 실행 함수 (스킬 유형에 따라 선택 가능)
    void ExecuteAction(const FSkillAction& Action, AActor* Target);
    void ExecutePlayMontage(const FSkillAction& Action);
    void ExecuteSpawnProjectile(const FSkillAction& Action, AActor* Target);
    void ExecuteDashAction(const FSkillAction& Action, AActor* Target);
    void ExecuteSetVisibility(const FSkillAction& Action);

    // 공통 파티클 효과 처리
    void PlayActionEffects(const FSkillAction& Action);

    // 완료 처리
    void CompleteActionSequence();
    void UpdateDashMovement(void* DashDataPtr);

};
