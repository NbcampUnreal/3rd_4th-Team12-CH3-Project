// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemSpawnPoint.h"

#include "Engine/AssetManager.h"
#include "Item/STItemBase.h"
#include "Item/STItemSpawnList.h"


struct FStreamableManager;

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
	if (IsValid(ItemListToSpawn) && ItemListToSpawn->SpawnableItems.Num() > 0)
	{
		int32 SpawnNumber = FMath::RandRange(0, ItemListToSpawn->SpawnableItems.Num() - 1);
		return ItemListToSpawn->SpawnableItems[SpawnNumber];
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
		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		Streamable.RequestAsyncLoad(
			ItemClassToSpawnPtr.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda([this, ItemClassToSpawnPtr]() 
			{
				UClass* LoadedClass = ItemClassToSpawnPtr.Get(); 
				if (LoadedClass)
				{
					//world spawn 
					SpawnItem = GetWorld()->SpawnActor<ASTItemBase>(
						LoadedClass,
						GetActorLocation(),
						FRotator::ZeroRotator
					);
					// binding item
					if (IsValid(SpawnItem))
					{
						SpawnItem->OnDestroyed.AddDynamic(
							this, 
							&AItemSpawnPoint::DestroyedSpawnItem
						);
					}
				}
			})
		);
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




