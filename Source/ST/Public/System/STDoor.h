#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STDoor.generated.h"

UCLASS()
class ST_API ASTDoor : public AActor
{
	GENERATED_BODY()

public:
	ASTDoor();
	
	UPROPERTY( EditAnywhere, Category ="Door")
	bool bIsClosed;		// NOTE: bIsOpened가 더 직관적임(긍정상태값으로 네이밍 하는게 좋음)

	/* Blueprint Events */
	UFUNCTION( BlueprintImplementableEvent )
	void OpenDoor();
	
	UFUNCTION( BlueprintImplementableEvent )
	void CloseDoor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleBossDefeated(AActor* DeadEnemy);

private:
	UPROPERTY( EditAnywhere, Category ="Door", meta = ( AllowPrivateAccess = "true" ) )
	UStaticMeshComponent* DoorMesh;
};
