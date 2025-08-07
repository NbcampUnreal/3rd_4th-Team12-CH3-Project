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
	}
	else if (OwnerChar == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Owner NUll"));
	}
	else if(CurrentWeapon == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Weapon NUll"));
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

void USTWeaponManagerComponent::ReloadAmmo()
{
	if (IsValid(CurrentWeapon) && !CurrentWeapon->IsReloading())
	{
		CurrentWeapon->StartReload();
		OwnerChar->PlayReloadAnimation();
	}
}

void USTWeaponManagerComponent::UnequipWeapon()
{
	if (IsValid(CurrentWeapon))
	{
		//todo current weapon delete
	}
}




