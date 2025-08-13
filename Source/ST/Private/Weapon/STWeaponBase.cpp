// Copyright Epic Games, Inc. All Rights Reserved.

#include "Weapon/STWeaponBase.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Player/STPlayerCharacter.h"

// 생성자
ASTWeaponBase::ASTWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
	
	WeaponMesh3p=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh3p"));
	WeaponMesh3p->SetupAttachment(RootComponent);
}

// BeginPlay
void ASTWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	if (WeaponDataAsset)
	{
		const FSTWeaponData& Data = WeaponDataAsset->WeaponData;
		WeaponType = Data.WeaponType;
		Damage = Data.Damage;
		FireRate = Data.FireRate;
		MagazineSize = Data.MagazineSize;
		ReloadTime = Data.ReloadTime;
		SpreadAngle = Data.SpreadAngle;
		PelletsPerShot= Data.PelletsPerShot;
		CameraShakeScale = Data.CameraShakeScale;
		DefaultSpreadAngle = Data.SpreadAngle;
		CurrentAmmo = MagazineSize;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponDataAsset not assigned."));
	}

	if (OnWeaponEquipped.IsBound())
	{
		OnWeaponEquipped.Broadcast(WeaponName);
	}
	if (OnAmmoChanged.IsBound())
	{
		OnAmmoChanged.Broadcast(CurrentAmmo, MagazineSize);
	}
}

// Fire() - 공통 기능
void ASTWeaponBase::Fire()
{
	if (!bCanFire || CurrentAmmo <= 0)
	{
		if (CurrentAmmo <= 0) { UE_LOG(LogTemp, Warning, TEXT("No Ammo")); }
		return;
	}
	HandleFire();
}

// HandleFire() - 공통 기능만 남기고 수정
void ASTWeaponBase::HandleFire()
{
	// 1. 공통 기능 (사운드, 탄약 감소, 연사 타이머, 화면 흔들림, 총구 이펙트)
	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	
	bCanFire = false;
	CurrentAmmo--;
	OnAmmoChanged.Broadcast(CurrentAmmo, MagazineSize);
	
	GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, this, &ASTWeaponBase::EnableFire, 60.0f / FireRate, false);
	PlayFireCameraShake();

	if (MuzzleFlashEffect)
	{
		if (ASTPlayerCharacter* PlayerCharacter = Cast<ASTPlayerCharacter>(GetOwner()))
		{
			UStaticMeshComponent* ActiveWeaponMesh = PlayerCharacter->GetCurrentViewMode() == EViewMode::FPS ? WeaponMesh : WeaponMesh3p;
			UGameplayStatics::SpawnEmitterAttached(
				MuzzleFlashEffect,
				ActiveWeaponMesh,
				TEXT("MuzzleSocket")
			);
			PlayerCharacter->OnWeaponFired();
		}
	}

	// 2. ★★★ 핵심: 실제 발사 방식은 자식에게 위임 ★★★
	FireWeapon();
}

// FireWeapon() - 부모 클래스에서는 경고만 출력
void ASTWeaponBase::FireWeapon()
{
	// 이 함수는 자식 클래스에서 반드시 재정의(override)되어야 합니다.
	ensure(false && "FireWeapon() must be implemented in the child class.");
	UE_LOG(LogTemp, Error, TEXT("FireWeapon() is not implemented in the child class!"));
}

// --- [ PerformTrace()와 ProcessHit() 함수는 이 파일에서 완전히 삭제됨 ] ---

// --- [ 아래는 모두 공통 기능이므로 그대로 이 파일에 둡니다 ] ---
void ASTWeaponBase::EnableFire() { bCanFire = true; }

void ASTWeaponBase::StartReload()
{
	if (bIsReloading || CurrentAmmo == MagazineSize || GetWorld()->GetTimerManager().IsTimerActive(FireRateTimerHandle))
	{
		return;
	}

	if (ReloadSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, GetActorLocation());
	}
	bIsReloading = true;
	bCanFire = false;

	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ASTWeaponBase::FinishReload, ReloadTime, false);
}

void ASTWeaponBase::FinishReload()
{
	CurrentAmmo = MagazineSize;
	bIsReloading = false;
	bCanFire = true;

	if (OnAmmoChanged.IsBound())
	{
		OnAmmoChanged.Broadcast(CurrentAmmo, MagazineSize);
	}
}

EFireMode ASTWeaponBase::GetFireMode() const
{
	if (WeaponDataAsset)
	{
		return WeaponDataAsset->WeaponData.FireMode;
	}
	return EFireMode::Automatic;
}

void ASTWeaponBase::StartFire()
{
	const EFireMode Mode = GetFireMode();

	if (Mode == EFireMode::Automatic)
	{
		GetWorld()->GetTimerManager().SetTimer(AutoFireTimerHandle, this, &ASTWeaponBase::Fire, 60.0f / FireRate, true, 0.0f);
	}
	else if (Mode == EFireMode::SemiAutomatic)
	{
		Fire();
	}
}

void ASTWeaponBase::StopFire()
{
	GetWorld()->GetTimerManager().ClearTimer(AutoFireTimerHandle);
}

void ASTWeaponBase::ToggleFireMode()
{
	// ToggleFireMode는 특정 무기 타입(Rifle)에 종속된 로직이므로,
	// 나중에 Rifle 자식 클래스 등으로 옮기는 것을 고려해볼 수 있습니다.
	// 지금은 일단 여기에 둡니다.
	if (!WeaponDataAsset) return;

	// if (WeaponDataAsset->WeaponData.WeaponType != EWeaponType::Rifle)
	// {
	// 	return;
	// }
	
	EFireMode& CurrentMode = WeaponDataAsset->WeaponData.FireMode;
	if (CurrentMode == EFireMode::Automatic)
	{
		CurrentMode = EFireMode::SemiAutomatic;
	}
	else
	{
		CurrentMode = EFireMode::Automatic;
	}
}

void ASTWeaponBase::PlayFireCameraShake()
{
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController()))
	{
		if (FireCameraShake)
		{
			PC->ClientStartCameraShake(FireCameraShake, CameraShakeScale);
		}
	}
}

void ASTWeaponBase::StartAiming()
{
	bIsAiming = true;

	if (WeaponDataAsset->WeaponData.WeaponType == EWeaponType::Shotgun)
	{
	SpreadAngle = SpreadAngle / 2.0f;
	}
	 else
	{
	 	SpreadAngle = 0.0f;
	}
}

void ASTWeaponBase::StopAiming()
{
	bIsAiming = false;
	SpreadAngle = DefaultSpreadAngle;
}

bool ASTWeaponBase::IsFiring() const
{
	return GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(AutoFireTimerHandle);
}