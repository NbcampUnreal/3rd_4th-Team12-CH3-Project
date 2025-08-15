// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemSpawnPoint.h"

#include "Item/STItemBase.h"


AItemSpawnPoint::AItemSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AItemSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
	HandleSpawnItem();
}

TSoftClassPtr<ASTItemBase> AItemSpawnPoint::GetSpawningItem()
{
	if (SpawnItems.Num() > 0)
	{
		int32 SpawnNumber = FMath::RandRange(0, SpawnItems.Num() - 1);
		return SpawnItems[SpawnNumber];
	}
	return nullptr;
}

void AItemSpawnPoint::HandleSpawnItem()
{
	
	if (IsValid(SpawnItem))
	{
		return;
	}

	TSoftClassPtr<ASTItemBase> ItemClassToSpawnPtr = GetSpawningItem();
	if (ItemClassToSpawnPtr.IsValid())
	{
		UClass* LoadedClass = ItemClassToSpawnPtr.LoadSynchronous();
		if (LoadedClass)
		{
			
			SpawnItem = GetWorld()->SpawnActor<ASTItemBase>(LoadedClass, GetActorLocation(), FRotator::ZeroRotator);

			// 스폰된 아이템에 델리게이트를 바인딩합니다.
			if (IsValid(SpawnItem))
			{
				SpawnItem->OnDestroyed.AddDynamic(this, &AItemSpawnPoint::DestroyedSpawnItem);
			}
		}
	}
}


void AItemSpawnPoint::DestroyedSpawnItem(AActor* DestroyedActor)
{

	if (SpawnItem != DestroyedActor)
	{
		return;
	}
	SpawnItem = nullptr;
	SetSpawnTimer();
}

void AItemSpawnPoint::SetSpawnTimer()
{

	if (SpawnTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
	}
	
	GetWorld()->GetTimerManager().SetTimer(
		SpawnTimerHandle,
		this,
		&AItemSpawnPoint::HandleSpawnItem,
		SpawnTimer, 
		false  
	);
}




