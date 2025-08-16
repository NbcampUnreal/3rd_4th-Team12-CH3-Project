#include "Enemy/STEnemyBossAIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"

ASTEnemyBossAIController::ASTEnemyBossAIController()
{
    if (SightConfig)
    {
        SightConfig->SightRadius = 3000.f;
        SightConfig->LoseSightRadius = 3500.f;
        SightConfig->PeripheralVisionAngleDegrees = 80.f;
        SightConfig->SetMaxAge(30.f);
    }
    if (HearingConfig)
    {
        HearingConfig->HearingRange = 3000.f;
    }

	bHasRecognizedPlayer = false;
}

void ASTEnemyBossAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// 부모 클래스 호출로 RecognitionGauge 업데이트
	Super::OnPerceptionUpdated(Actor, Stimulus);

	if (RecognitionGauge >= 100.f && !bHasRecognizedPlayer)
	{
		bHasRecognizedPlayer = true;
		OnBossRecognizedPlayer.Broadcast();
        
	}
}
