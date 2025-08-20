#include "Enemy/STEnemyRangedAIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"

ASTEnemyRangedAIController::ASTEnemyRangedAIController()
{
	if (SightConfig)
	{
		SightConfig->SightRadius = 2000.f;
		SightConfig->LoseSightRadius = 2200.f;
		SightConfig->PeripheralVisionAngleDegrees = 80.f;
		SightConfig->SetMaxAge(6.f);
	}
	if (HearingConfig)
	{
		HearingConfig->HearingRange = 2000.f;
	}
}
