#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "STUIGameModeBase.generated.h"

/**
 *  메인메뉴, 로비화면 전용 GameModeBase
 */
UCLASS()
class ST_API ASTUIGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASTUIGameModeBase();
};
