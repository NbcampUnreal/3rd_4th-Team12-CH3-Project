// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/STWeaponManagerComponent.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Item/STItemPivotData.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/STPlayerCharacter.h"
#include "Player/ST_PlayerAnimMontageConfig.h"
#include "Weapon/STWeaponBase.h"

#pragma region Initialization
// Sets default values for this component's properties
USTWeaponManagerComponent::USTWeaponManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USTWeaponManagerComponent::BeginPlay()
{
    Super::BeginPlay();
    if (OwnerChar == nullptr)
    {
       OwnerChar = Cast<ASTPlayerCharacter>(GetOwner());
       if (OwnerChar)
       {
          OwnerChar->OnViewModeChanged.AddUObject(this, &USTWeaponManagerComponent::UpdateWeaponVisibility);
       }
    }
}
#pragma endregion

#pragma region Weapon Management
void USTWeaponManagerComponent::RequestEquipWeapon(TSoftClassPtr<ASTWeaponBase> WeaponClass)
{
    
    if (!OwnerChar )
    {
       return;
    }
    if (WeaponClass.IsNull()|| WeaponClass == CurrentWeaponClass)
    {
       return;
    }

    PendingWeaponClass = WeaponClass;
    if (PendingWeaponClass.IsNull())
    {
       return;
    }
    
    bIsWeaponChanged = true;
    UAnimInstance* AnimInstance = OwnerChar->GetMesh()->GetAnimInstance();
    UAnimInstance* FPSAnimInstance = OwnerChar->GetFPSSkeletalMesh()->GetAnimInstance();
    UAnimMontage* EquipMontage = OwnerChar->GetMontageConfig()->EquipMontage;

    if (!AnimInstance&&!FPSAnimInstance)
    {
       return;
    }
    if (!EquipMontage)
    {
       return;
    }
    AnimInstance->Montage_Play(EquipMontage);
    FPSAnimInstance->Montage_Play(EquipMontage);
    MontageEndedDelegate.BindUObject(this, &USTWeaponManagerComponent::OnEquipMontageEnded);
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, EquipMontage);
	FPSAnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, EquipMontage);
    
    
}

EWeaponType USTWeaponManagerComponent::GetCurrentWeaponType()
{
    if (CurrentWeapon)
    {
       return CurrentWeapon->WeaponDataAsset->WeaponData.WeaponType;
    }
    return EWeaponType::Rifle;
}


void USTWeaponManagerComponent::OnEquipMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (bInterrupted)
    {
       return;
    }
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(
		PendingWeaponClass.ToSoftObjectPath(),
		FStreamableDelegate::CreateUObject(this, &USTWeaponManagerComponent::EquipWeapon, PendingWeaponClass)
	);
}

void USTWeaponManagerComponent::EquipWeapon(TSoftClassPtr<ASTWeaponBase> WeaponClass)
{
    if (WeaponClass.IsNull())
    {
        bIsWeaponChanged = false;
        return;
    }
    
    UClass* WeaponClassToSpawn = WeaponClass.Get();
    
    if (!WeaponClassToSpawn)
    {
        bIsWeaponChanged = false;
        return;
    }
    
    ASTWeaponBase* NewWeapon = GetWorld()->SpawnActor<ASTWeaponBase>(WeaponClassToSpawn);
    if (!NewWeapon)
    {
        bIsWeaponChanged = false;
        return;
    }
    
    if (CurrentWeapon)
    {
        UnequipWeapon();
    }
    
    CurrentWeapon = NewWeapon;
    CurrentWeaponClass = WeaponClass;
    CurrentWeapon->SetOwner(GetOwner());

    // 메시 충돌 비활성화
    if (CurrentWeapon->GetWeaponMesh1P())
    {
        CurrentWeapon->GetWeaponMesh1P()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    if (CurrentWeapon->GetWeaponMesh3P())
    {
        CurrentWeapon->GetWeaponMesh3P()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    // 캐릭터에 무기 부착 및 설정
    if (OwnerChar && CurrentWeapon)
    {
        CurrentWeapon->GetWeaponMesh1P()->AttachToComponent(
            OwnerChar->GetFPSSkeletalMesh(),
            FAttachmentTransformRules::SnapToTargetIncludingScale,
            AttachSocket1P
        );
        CurrentWeapon->GetWeaponMesh1P()->SetWorldScale3D(FVector(0.6f, 0.6f, 0.6f));

        CurrentWeapon->GetWeaponMesh3P()->AttachToComponent(
            OwnerChar->GetMesh(),
            FAttachmentTransformRules::SnapToTargetIncludingScale,
            AttachSocket3P
        );
        UpdateWeaponSocketOffset(CurrentWeapon->WeaponDataAsset->WeaponData.WeaponType);
        UpdateWeaponVisibility(OwnerChar->GetCurrentViewMode());
        
        CurrentWeapon->OnWeaponEquipped.AddDynamic(this, &USTWeaponManagerComponent::OnWeaponEquipped);
        CurrentWeapon->OnAmmoChanged.AddDynamic(this, &USTWeaponManagerComponent::OnWeaponAmmoChanged);

        bIsWeaponChanged = false;
        if (EquipChangedDelegate.IsBound())
        {
           EquipChangedDelegate.Broadcast(CurrentWeaponClass);
        }
        EWeaponType WeaponType = CurrentWeapon->WeaponDataAsset->WeaponData.WeaponType;

        FString WeaponTypeName = StaticEnum<EWeaponType>()
          ->GetNameStringByValue(static_cast<int64>(WeaponType));

        OnWeaponEquipped(WeaponTypeName);
        OnWeaponAmmoChanged(
          CurrentWeapon->WeaponDataAsset->WeaponData.MagazineSize,
          CurrentWeapon->WeaponDataAsset->WeaponData.MagazineSize
       );
    }
	PendingWeaponClass = nullptr;
}
void USTWeaponManagerComponent::UnequipWeapon()
{
    if (IsValid(CurrentWeapon))
    {
       //binding 홰제
       CurrentWeapon->OnWeaponEquipped.RemoveDynamic(this, &USTWeaponManagerComponent::OnWeaponEquipped);
       CurrentWeapon->OnAmmoChanged.RemoveDynamic(this, &USTWeaponManagerComponent::OnWeaponAmmoChanged);

       //부착 해제 
       CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
       //제거
       CurrentWeapon->Destroy();
       CurrentWeapon = nullptr;
    }
}
#pragma endregion


#pragma region Firing and Aiming
bool USTWeaponManagerComponent::CanFireWeapon()
{
    if (CurrentWeapon == nullptr)
    {
       return false;
    }
    if (CurrentWeapon->IsReloading())
    {
       return false;
    }
    if (bIsWeaponChanged)
    {
       return false;
    }

    return true;
}

void USTWeaponManagerComponent::StartFire()
{
    if (CanFireWeapon())
    {
       CurrentWeapon->StartFire();
    }
}

void USTWeaponManagerComponent::StopFire()
{
    if (IsValid(CurrentWeapon))
    {
       CurrentWeapon->StopFire();
    }
}

void USTWeaponManagerComponent::StartAiming()
{
    if (IsValid(CurrentWeapon))
    {
       CurrentWeapon->StartAiming();
    }
       
}

void USTWeaponManagerComponent::StopAiming()
{
    if (IsValid(CurrentWeapon))
    {
       CurrentWeapon->StopAiming();
    }
}

void USTWeaponManagerComponent::ReloadAmmo()
{
    if (IsValid(CurrentWeapon) && !CurrentWeapon->IsReloading()&&(CurrentWeapon->GetCurrentAmmo() != CurrentWeapon->WeaponDataAsset->WeaponData.MagazineSize))
    {
       StopFire();
       OwnerChar->PlayReloadAnimation();
       CurrentWeapon->StartReload();
       
    }
}
#pragma endregion

#pragma region Utility and Callbacks
FStItemPivotData* USTWeaponManagerComponent::GetWeaponPivotData(EWeaponType type)
{
    if (IsValid(SocketOffsetTable))
    {
       FString WeaponRow;
       switch(type)
       {
       case EWeaponType::Rifle:
          {
             WeaponRow = TEXT("Rifle");
             break;
          }
       case EWeaponType::Shotgun:
          {
             WeaponRow = TEXT("Shotgun");
             break;
          }
       case EWeaponType::Sniper:
          {
             WeaponRow = TEXT("Sniper");
             break;
          }
       case EWeaponType::Launcher:
       	{
       		WeaponRow = TEXT("Launcher");
       		break;
       	}
       default:
          return nullptr;
       }
       return SocketOffsetTable->FindRow<FStItemPivotData>(*WeaponRow, TEXT(""));
    }
    return nullptr;
}

void USTWeaponManagerComponent::UpdateWeaponSocketOffset(EWeaponType type)
{

    FStItemPivotData* WeaponPivotData = GetWeaponPivotData(type);
    if (WeaponPivotData != nullptr)
    {
       if (CurrentWeapon)
       {
          CurrentWeapon->GetWeaponMesh3P()->SetRelativeLocation(WeaponPivotData->PositionOffset);
          CurrentWeapon->GetWeaponMesh3P()->SetRelativeRotation(WeaponPivotData->RotationOffset);

          CurrentWeapon->GetWeaponMesh1P()->SetRelativeLocation(WeaponPivotData->FPSPositionOffset);
          CurrentWeapon->GetWeaponMesh1P()->SetRelativeRotation(WeaponPivotData->FPSRotationOffset);
       }
    }

}
void USTWeaponManagerComponent::UpdateWeaponVisibility(EViewMode NewMode) // Visible View Mode
{
    if (CurrentWeapon)
    {
       bool bThirdPerson = (NewMode== EViewMode::TPS);
       CurrentWeapon->GetWeaponMesh1P()->SetVisibility(!bThirdPerson);
       CurrentWeapon->GetWeaponMesh3P()->SetVisibility(bThirdPerson);
    }
 }
void USTWeaponManagerComponent::OnWeaponEquipped(const FString& WeaponName) 
{
    if (EquipDelegate.IsBound())
    {
       EquipDelegate.Broadcast(WeaponName);
    }
}

void USTWeaponManagerComponent::OnWeaponAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo)
{
    if (AmmoChangeDelegate.IsBound())
    {
       AmmoChangeDelegate.Broadcast(CurrentAmmo, MaxAmmo);
    }
}

#pragma endregion