#include "Enemy/BTDecorator_HasValidSkill.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTDecorator_HasValidSkill::UBTDecorator_HasValidSkill()
{
	NodeName = TEXT("Has Valid Skill");
	bAllowAbortLowerPri = true;
	bAllowAbortNone = false;
    
	HasValidSkillKey.SelectedKeyName = TEXT("HasValidSkill");
}

bool UBTDecorator_HasValidSkill::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return false;

	bool bHasValidSkill = BB->GetValueAsBool(HasValidSkillKey.SelectedKeyName);
    
	return bHasValidSkill;
}
