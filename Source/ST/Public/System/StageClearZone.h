#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StageClearZone.generated.h"

class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerEnteredClearZone);

UCLASS()
class ST_API AStageClearZone : public AActor
{
	GENERATED_BODY()

public:
	AStageClearZone();

	UPROPERTY( BlueprintAssignable, Category = "Event" );
	FOnPlayerEnteredClearZone OnPlayerEnteredClearZone;

protected:
	UPROPERTY( VisibleAnywhere )
	USceneComponent* Scene;
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	USphereComponent* Trigger;

	UPROPERTY( VisibleAnywhere )
	UStaticMeshComponent* StaticMesh;
	
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
};
