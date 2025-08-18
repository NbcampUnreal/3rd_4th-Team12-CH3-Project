#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StageClearZone.generated.h"

class USphereComponent;
class UStaticMeshComponent;

/* Event Delegate */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerEnteredClearZone);

UCLASS()
class ST_API AStageClearZone : public AActor
{
	GENERATED_BODY()

public:
	AStageClearZone();

	/* 플레이어가 클리어 존에 들어갈 때 호출 */
	UPROPERTY( BlueprintAssignable, Category = "Event" )
	FOnPlayerEnteredClearZone OnPlayerEnteredClearZone;

protected:
	UPROPERTY( VisibleAnywhere )
	USceneComponent* Scene;		// Root Scene Component
	
	UPROPERTY( EditAnywhere, BlueprintReadWrite )
	USphereComponent* Trigger;	// 플레이어 감지용 Trigger

	UPROPERTY( VisibleAnywhere )
	UStaticMeshComponent* StaticMesh;	// 클리어존 시각 표현용 Mesh

protected:
	/* AActor 인터페이스 */
	virtual void BeginPlay() override;

	/* 트리거 Overlap 시작 이벤트 */
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
