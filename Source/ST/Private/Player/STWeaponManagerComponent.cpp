// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/STWeaponManagerComponent.h"

#include "Player/STPlayerCharacter.h"
#include "Weapon/STWeaponBase.h"

// Sets default values for this component's properties
USTWeaponManagerComponent::USTWeaponManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}
void USTWeaponManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();

	
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
	if (CurrentWeapon == nullptr && IsValid(DefaultWeapon))
	{
		EquipWeapon(DefaultWeapon);
	}
}

void USTWeaponManagerComponent::EquipWeapon(TSubclassOf<ASTWeaponBase> WeaponClass)
{
	if (CurrentWeapon)
	{
		UnequipWeapon();
	}
    
	ASTWeaponBase* NewWeapon = GetWorld()->SpawnActor<ASTWeaponBase>(WeaponClass);
	CurrentWeapon = NewWeapon;
	CurrentWeapon->SetOwner(GetOwner());
	if (CurrentWeapon)
	{
		if (CurrentWeapon->GetWeaponMesh1P())
		{
			CurrentWeapon->GetWeaponMesh1P()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			
			
		}
		if (CurrentWeapon->GetWeaponMesh3P())
		{
			CurrentWeapon->GetWeaponMesh3P()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	
	if (OwnerChar && CurrentWeapon)
	{
		//fps
		CurrentWeapon->GetWeaponMesh1P()->AttachToComponent(
		OwnerChar->GetFPSSkeletalMesh(),
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		AttachSocket1P
	);
		CurrentWeapon->GetWeaponMesh1P()->SetWorldScale3D(FVector(0.6f, 0.6f, 0.6f));
     
		//tps
		CurrentWeapon->GetWeaponMesh3P()->AttachToComponent(
			OwnerChar->GetMesh(), 
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			AttachSocket3P
		);
     
		UpdateWeaponVisibility(OwnerChar->GetCurrentViewMode());

		//binding
		CurrentWeapon->OnWeaponEquipped.AddDynamic(this, &USTWeaponManagerComponent::OnWeaponEquipped);
		CurrentWeapon->OnAmmoChanged.AddDynamic(this, &USTWeaponManagerComponent::OnWeaponAmmoChanged);
	}
	if (IsValid(OwnerChar))
	{
		OwnerChar->OnWeaponEquipped(CurrentWeapon->WeaponDataAsset->WeaponData.WeaponType);
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



void USTWeaponManagerComponent::StartFire()
{
	if (IsValid(CurrentWeapon))
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
	if (IsValid(CurrentWeapon) && !CurrentWeapon->IsReloading())
	{
		OwnerChar->PlayReloadAnimation();
		CurrentWeapon->StartReload();
		
	}
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



