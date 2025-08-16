#include "System/STBossRoomTrigger.h"

#include "Player/STPlayerCharacter.h"
#include "System/STLog.h"


ASTBossRoomTrigger::ASTBossRoomTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASTBossRoomTrigger::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if (Cast<ASTPlayerCharacter>(OtherActor) && DoorToClose)
	{
		if (!DoorToClose->bIsClosed)
		{
			DoorToClose->CloseDoor();
			DoorToClose->bIsClosed = true;
			UE_LOG(LogSystem, Warning, TEXT("ASTBossRoomTrigger::NotifyActorBeginOverlap Door Closed"));			
			Destroy();
		}
	}
	else
	{
		UE_LOG(LogSystem, Warning, TEXT("ASTBossRoomTrigger::NotifyActorBeginOverlap Can't Cast"));
	}
}
