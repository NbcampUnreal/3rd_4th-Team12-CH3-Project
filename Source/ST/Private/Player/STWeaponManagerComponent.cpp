// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/STWeaponManagerComponent.h"

#include "Weapon/STWeaponBase.h"

// Sets default values for this component's properties
USTWeaponManagerComponent::USTWeaponManagerComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

	
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
    
 //     //fps
	// CurrentWeapon->GetFirstPersonMesh()->AttachToComponent(
	// 	OwnerChar->GetFirstPersonArms(),
	// 	FAttachmentTransformRules::SnapToTargetIncludingScale,
	// 	TEXT("hand_r_socket")
	// );
 //    
	// //tps
	// CurrentWeapon->GetThirdPersonMesh()->AttachToComponent(
	// 	OwnerChar->GetMesh(), 
	// 	FAttachmentTransformRules::SnapToTargetIncludingScale,
	// 	TEXT("RightHandWeapon")
	// );
 //    
	// UpdateWeaponVisibility();
}

void USTWeaponManagerComponent::UpdateWeaponVisibility()
{
// 	if (CurrentWeapon)
// 	{
// 		bool bFirstPerson = OwnerChar->IsFirstPerson();
// 		CurrentWeapon->GetFirstPersonMesh()->SetVisibility(bFirstPerson);
// 		CurrentWeapon->GetThirdPersonMesh()->SetVisibility(!bFirstPerson);
// 	}
 }

void USTWeaponManagerComponent::UnequipWeapon()
{
	
}




