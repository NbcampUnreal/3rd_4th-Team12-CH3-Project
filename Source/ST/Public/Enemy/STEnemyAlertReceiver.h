#pragma once

#include "UObject/Interface.h"
#include "STEnemyAlertReceiver.generated.h"

UINTERFACE(MinimalAPI)
class USTEnemyAlertReceiver : public UInterface
{
	GENERATED_BODY()
};

class ISTEnemyAlertReceiver
{
	GENERATED_BODY()

public:
	virtual void NotifyPlayerAttack(AActor* DamagedEnemy) = 0;
};
