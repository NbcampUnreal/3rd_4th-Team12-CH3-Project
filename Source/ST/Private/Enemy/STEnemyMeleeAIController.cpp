#include "Enemy/STEnemyMeleeAIController.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"

ASTEnemyMeleeAIController::ASTEnemyMeleeAIController()
{
	if (SightConfig)
	{
		SightConfig->SightRadius = 2000.f; 
		SightConfig->LoseSightRadius = 2200.f;
		SightConfig->PeripheralVisionAngleDegrees = 90.f;
		SightConfig->SetMaxAge(6.0f);
	}
}
