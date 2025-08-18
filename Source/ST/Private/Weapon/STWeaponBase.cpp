// Copyright Epic Games, Inc. All Rights Reserved.

#include "Weapon/STWeaponBase.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Player/STPlayerCharacter.h"

// 생성자
ASTWeaponBase::ASTWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;


	//1인칭용 메쉬설정
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	//3인칭용 메쉬설정
	WeaponMesh3p=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh3p"));
	WeaponMesh3p->SetupAttachment(RootComponent);
}

// BeginPlay
void ASTWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	if (WeaponDataAsset)
	{
		//데이터 에셋을 통해 기본 총기 데이터 설정
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
		//데이터 에셋을 찾지 못한 경우
		UE_LOG(LogTemp, Warning, TEXT("WeaponDataAsset not assigned."));
	}

	//총기 생성시 델리게이트를 통해 탄약 정보 방송
	if (OnWeaponEquipped.IsBound())
	{
		OnWeaponEquipped.Broadcast(WeaponName);
	}
	if (OnAmmoChanged.IsBound())
	{
		OnAmmoChanged.Broadcast(CurrentAmmo, MagazineSize);
	}
}

void ASTWeaponBase::Fire()
{
	//탄약 및 총기 발사 딜레이 확인을 통해 발사 가능여부 판단
	if (!bCanFire || CurrentAmmo <= 0)
	{
		if (CurrentAmmo <= 0) { UE_LOG(LogTemp, Warning, TEXT("No Ammo")); }
		return;
	}
	//실제 발사 호출
	HandleFire();
}


void ASTWeaponBase::HandleFire()
{

	// 공통 기능구현 사운드, 탄약 감소, 연사 타이머, 화면 흔들림, 총구 이펙트

	//총기 소리 발생
	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());

	//장전 딜레이용 변수 값 설정
	bCanFire = false;
	//탄약 감소
	CurrentAmmo--;
	//감소된 탄약을 델리게이트를 통해 방송
	OnAmmoChanged.Broadcast(CurrentAmmo, MagazineSize);
	//60/FireRate 초 뒤에 발사 가능 함수 호출
	GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, this, &ASTWeaponBase::EnableFire, 60.0f / FireRate, false);
	//카메라 쉐이크 호출
	PlayFireCameraShake();

	//머즐 이펙트
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

	//자식 클래스에서 재정의할 FireWeapon
	FireWeapon();
}

void ASTWeaponBase::FireWeapon()
{
}

//총기 딜레이 함수
void ASTWeaponBase::EnableFire() { bCanFire = true; }


//장전 함수
void ASTWeaponBase::StartReload()
{
	//장전중 또는 탄약이 가득 차 있거나 총기 딜레이 함수 즉 총을 발사하고 있을때는 장전 불가능
	if (bIsReloading || CurrentAmmo == MagazineSize || GetWorld()->GetTimerManager().IsTimerActive(FireRateTimerHandle))
	{
		return;
	}

	//장전 사운드
	if (ReloadSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, GetActorLocation());
	}

	//장전 중 및 발사 불가능 처리
	bIsReloading = true;
	bCanFire = false;

	//reloadTime 후 실제 장전 완료 함수 호출
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ASTWeaponBase::FinishReload, ReloadTime, false);
}


void ASTWeaponBase::FinishReload()
{
	//탄약 채움 및 장전 완료 발사 가능 처리
	CurrentAmmo = MagazineSize;
	bIsReloading = false;
	bCanFire = true;

	//탄약 정보 델리게이트 방송
	if (OnAmmoChanged.IsBound())
	{
		OnAmmoChanged.Broadcast(CurrentAmmo, MagazineSize);
	}
}

//발사 모드 게터
EFireMode ASTWeaponBase::GetFireMode() const
{
	if (WeaponDataAsset)
	{
		return WeaponDataAsset->WeaponData.FireMode;
	}
	return EFireMode::Automatic;
}

//단발 연발 조정 발사 로직
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

//마우스를 때는 시점에 호출되는 발사 정지 함수
void ASTWeaponBase::StopFire()
{
	GetWorld()->GetTimerManager().ClearTimer(AutoFireTimerHandle);
}


//쓸지 모르는 총기 사격 모드 변경 함수
void ASTWeaponBase::ToggleFireMode()
{

	if (!WeaponDataAsset) return;

	//일단 라이플만 가능
	if (WeaponDataAsset->WeaponData.WeaponType != EWeaponType::Rifle)
	{
		return;
	}
	
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

//카메라 쉐이크 함수
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

//조준시 총기 정확도 향상 함수
void ASTWeaponBase::StartAiming()
{
	bIsAiming = true;

	if (WeaponDataAsset->WeaponData.WeaponType == EWeaponType::Shotgun)
	{
	SpreadAngle = SpreadAngle / 1.5f;
	}
	 else
	{
	 	SpreadAngle = 0.0f;
	}
}

//종료시 원래 정확도로 바꿔주는 함수
void ASTWeaponBase::StopAiming()
{
	bIsAiming = false;
	SpreadAngle = DefaultSpreadAngle;
}

//발사중인지 확인하는 함수
bool ASTWeaponBase::IsFiring() const
{
	return GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(AutoFireTimerHandle);
}