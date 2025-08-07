#include "Enemy/STEnemyMeleeAIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"

ASTEnemyMeleeAIController::ASTEnemyMeleeAIController()
{
	if (SightConfig)
	{
		SightConfig->SightRadius = 1200.f; 
		SightConfig->LoseSightRadius = 1400.f;
		SightConfig->PeripheralVisionAngleDegrees = 90.f;
		SightConfig->SetMaxAge(6.0f);
	}
}
