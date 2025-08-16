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
	bool bIsClosed;

	/* member functions */
	UFUNCTION( BlueprintImplementableEvent )
	void OpenDoor();
	UFUNCTION( BlueprintImplementableEvent )
	void CloseDoor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleBossDefeated(AActor* DeadEnemy);

private:
	UPROPERTY( EditAnywhere, Category ="Door")
	UStaticMeshComponent* DoorMesh;
};
