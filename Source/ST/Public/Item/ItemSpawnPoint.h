// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawnPoint.generated.h"

class ASTItemBase;

UCLASS()
class ST_API AItemSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemSpawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void HandleSpawnItem();

	void SetSpawnTimer();

	TSoftClassPtr<ASTItemBase> GetSpawningItem();

	UFUNCTION()
	void DestroyedSpawnItem(AActor* DestroyedActor);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	float SpawnTimer = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting")
	TArray<TSoftClassPtr<ASTItemBase>> SpawnItems;
	
private:
	
	FTimerHandle SpawnTimerHandle;
	
	TObjectPtr<ASTItemBase> SpawnItem;

	
	
	
	

};
