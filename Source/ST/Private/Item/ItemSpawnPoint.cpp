#include "Item/ItemSpawnPoint.h"
#include "Engine/AssetManager.h"
#include "Item/STItemBase.h"

AItemSpawnPoint::AItemSpawnPoint()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AItemSpawnPoint::BeginPlay()
{
    Super::BeginPlay();
    HandleSpawnItem();
}

TSoftClassPtr<ASTItemBase> AItemSpawnPoint::GetSpawningItem()
{
    if (SpawnableItems.Num() > 0)
    {
        int32 SpawnNumber = FMath::RandRange(0, SpawnableItems.Num() - 1);
        return SpawnableItems[SpawnNumber];
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
    if (ItemClassToSpawnPtr.IsNull())
    {
        return;
    }
    
    if (UClass* LoadedClass = ItemClassToSpawnPtr.Get())
    {
        SpawnItemDirectly(LoadedClass);
    }
    else
    {
        
        FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
        FStreamableDelegate LoadDelegate;
        
        LoadDelegate.BindLambda([this, ItemClassToSpawnPtr]() 
        {
            if (UClass* LoadedClass = ItemClassToSpawnPtr.Get())
            {
                SpawnItemDirectly(LoadedClass);
            }
        });
        Streamable.RequestAsyncLoad(ItemClassToSpawnPtr.ToSoftObjectPath(), LoadDelegate);
    }
}

void AItemSpawnPoint::SpawnItemDirectly(UClass* ItemClass)
{
    if (!ItemClass)
    {
        return;
    }
    
    SpawnItem = GetWorld()->SpawnActor<ASTItemBase>(
        ItemClass,
        GetActorLocation(),
        FRotator::ZeroRotator
    );
    
    if (IsValid(SpawnItem))
    {
        SpawnItem->OnDestroyed.AddDynamic(this, &AItemSpawnPoint::DestroyedSpawnItem);
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