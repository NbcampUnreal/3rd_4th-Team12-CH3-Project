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
}
