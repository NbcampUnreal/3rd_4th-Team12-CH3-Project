#include "Enemy/BTService_UpdateDistanceAndCooldown.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/STEnemyBoss.h"
#include "Enemy/STEnemySkillComponent.h"

UBTService_UpdateDistanceAndCooldown::UBTService_UpdateDistanceAndCooldown()
{
    NodeName = TEXT("Pick Skill");
    bNotifyBecomeRelevant = true;

    bTickIntervals = true;
    Interval = 0.5f;
    RandomDeviation = 0.1f;
}

void UBTService_UpdateDistanceAndCooldown::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UpdateSkillSelection(OwnerComp);
}

void UBTService_UpdateDistanceAndCooldown::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    UpdateSkillSelection(OwnerComp);
}

void UBTService_UpdateDistanceAndCooldown::UpdateSkillSelection(UBehaviorTreeComponent& OwnerComp)
{
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return;

    ASTEnemyBoss* Boss = Cast<ASTEnemyBoss>(OwnerComp.GetAIOwner()->GetPawn());
    if (!Boss || !Boss->SkillComponent) return;

	if (Boss->StateComponent &&
		(Boss->StateComponent->IsInState(EEnemyState::PhaseChanging) ||
		 Boss->StateComponent->IsInState(EEnemyState::Dead)))
	{
		BB->SetValueAsEnum(SkillToUseKey.SelectedKeyName, static_cast<uint8>(EEnemySkillID::None));
		BB->SetValueAsBool(TEXT("HasValidSkill"), false);
		return;
	}

    AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
    float Distance = Target ? FVector::Dist(Boss->GetActorLocation(), Target->GetActorLocation()) : FLT_MAX;
    BB->SetValueAsFloat(DistanceToTargetKey.SelectedKeyName, Distance);

    // 캐시된 스킬 리스트 사용
    EEnemySkillID SelectedSkillID = SelectBestSkillByCooldown(Boss, Distance);
    BB->SetValueAsEnum(SkillToUseKey.SelectedKeyName, static_cast<uint8>(SelectedSkillID));
    
    // 해당 스킬 실행 가능 여부 체크
    bool bCanAttack = (SelectedSkillID != EEnemySkillID::None) && 
                     Boss->SkillComponent->CanExecuteAnySkill();
    
    BB->SetValueAsBool(TEXT("HasValidSkill"), bCanAttack);
}

EEnemySkillID UBTService_UpdateDistanceAndCooldown::SelectBestSkillByCooldown(ASTEnemyBoss* Boss, float Distance)
{
    if (!Boss || !Boss->SkillComponent) return EEnemySkillID::None;

	if (Boss->StateComponent &&
		(Boss->StateComponent->IsInState(EEnemyState::PhaseChanging) ||
		 Boss->StateComponent->IsInState(EEnemyState::Dead)))
	{
		return EEnemySkillID::None;
	}
	
    // 현재 페이즈에 사용 가능한 스킬
    TArray<EEnemySkillID> AvailableSkills = Boss->GetCurrentPhaseSkills();

    // 캐시된 스킬 리스트 사용 (데이터테이블에서 쿨타임 순으로 정렬됨)
    const TArray<EEnemySkillID>& SkillsByPriority = Boss->SkillComponent->GetSkillsByPriority();

    // 사정거리와 쿨타임을 모두 만족하는 첫 번째 스킬 선택
    for (EEnemySkillID SkillID : SkillsByPriority)
    {
        // 현재 페이즈에서 사용 가능한 스킬인지 확인
        if (!AvailableSkills.Contains(SkillID)) continue;
        
        const FEnemySkillData* SkillData = Boss->SkillComponent->GetCachedSkillData(SkillID);
        if (!SkillData) continue;

        // 사정거리 체크
        bool bInRange = (SkillData->Range <= 0.0f) || (Distance <= SkillData->Range);
        
        // 개별 스킬 쿨타임 체크
        bool bReady = Boss->SkillComponent->IsSkillReady(SkillID);
        
        if (bInRange && bReady)
        {
            return SkillID;
        }
    }

    return EEnemySkillID::None;
}
