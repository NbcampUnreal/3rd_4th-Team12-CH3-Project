#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "STEnemyRangedAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UBlackboardComponent;
class UBehaviorTree;

UCLASS()
class ST_API ASTEnemyRangedAIController : public AAIController
{
	GENERATED_BODY()

public:
	ASTEnemyRangedAIController();

	FORCEINLINE UBlackboardComponent* GetBlackboardComponent() const
	{
		return BlackboardComp;
	}
	void StartBehaviorTree();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTreeAsset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* AIPerception;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAISenseConfig_Sight* SightConfig;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UBlackboardComponent* BlackboardComp;

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
