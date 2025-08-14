#pragma once

#include "CoreMinimal.h"
#include "Enemy/STAIControllerBase.h"
#include "STEnemyBossAIController.generated.h"

// 보스가 플레이어를 인식할 때 보스 체력 바를 보이게 하기 위한 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossRecognizedPlayer);

UCLASS()
class ST_API ASTEnemyBossAIController : public ASTAIControllerBase
{
	GENERATED_BODY()

public:
	ASTEnemyBossAIController();

	UPROPERTY(BlueprintAssignable, Category="Boss")
	FOnBossRecognizedPlayer OnBossRecognizedPlayer;

protected:
	// 플레이어를 처음 인식했는지 체크하는 플래그
	bool bHasRecognizedPlayer = false;

	virtual void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus) override;
};
