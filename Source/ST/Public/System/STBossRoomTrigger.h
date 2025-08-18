#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "STDoor.h"
#include "STBossRoomTrigger.generated.h"

UCLASS()
class ST_API ASTBossRoomTrigger : public ATriggerBox
{
	GENERATED_BODY()

public:
	ASTBossRoomTrigger();

	UPROPERTY( EditAnywhere, Category="Door" )
	ASTDoor* DoorToClose;

protected:
	virtual void NotifyActorBeginOverlap( AActor* OtherActor ) override;
};
