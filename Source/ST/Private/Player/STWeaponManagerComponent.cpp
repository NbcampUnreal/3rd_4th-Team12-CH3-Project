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
	//테스트 코드 
	if (OwnerChar && CurrentWeapon)
	{
		UStaticMeshComponent* WeaponMesh = CurrentWeapon->FindComponentByClass<UStaticMeshComponent>();
		USkeletalMeshComponent* MeshComp = OwnerChar->GetMesh();

		if (WeaponMesh && MeshComp)
		{
			// 소켓 존재 확인
			if (MeshComp->DoesSocketExist(AttachSocket3P))
			{
				WeaponMesh->AttachToComponent(
					MeshComp,
					FAttachmentTransformRules::SnapToTargetIncludingScale,
					AttachSocket3P
				);
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("Socket %s not found"), *AttachSocket3P.ToString()));
			}
		}
	}
	else if (OwnerChar == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Owner NUll"));
	}
	else if(CurrentWeapon == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Weapon NUll"));
	}
	
	//fps
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



void USTWeaponManagerComponent::StartFire()
{
	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->Fire();
	}
}

void USTWeaponManagerComponent::StopFire()
{
}

void USTWeaponManagerComponent::Reload()
{
}

void USTWeaponManagerComponent::ReloadAmmo()
{
	if (IsValid(CurrentWeapon))
	{
		CurrentWeapon->StartReload();
	}
}

void USTWeaponManagerComponent::UnequipWeapon()
{
	
}




