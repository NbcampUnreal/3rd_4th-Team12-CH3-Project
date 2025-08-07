// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/STWeaponBase.h"
#include "GameFramework/Character.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Player/STPlayerCharacter.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values
ASTWeaponBase::ASTWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	// 스태틱 메시 초기화
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
	
	WeaponMesh3p=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh3p"));

	WeaponMesh3p->SetupAttachment(RootComponent);


	//만약 무기끼리 충돌시 적용할 코드
	//충돌 없게 하는 코드
	//WeaponMesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	
}

// Called when the game starts or when spawned
void ASTWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	if (WeaponDataAsset)
	{
		//시작시 구조체 데이터를 통해 무기 기본값 설정
		const FSTWeaponData& Data = WeaponDataAsset->WeaponData;
		WeaponType = Data.WeaponType;
		Damage = Data.Damage;
		FireRate = Data.FireRate;
		MagazineSize = Data.MagazineSize;
		ReloadTime = Data.ReloadTime;
		SpreadAngle=Data.SpreadAngle;
		PelletsPerShot= Data.PelletsPerShot;
		CameraShakeScale=Data.CameraShakeScale;
		DefaultSpreadAngle = Data.SpreadAngle;

		
		CurrentAmmo = MagazineSize;

		UE_LOG(LogTemp, Log, TEXT("Weapon Initialized: %s, Damage: %.1f, Ammo: %d"),
	*UEnum::GetValueAsString(Data.WeaponType), Damage, MagazineSize);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponDataAsset not assigned."));
	}
	
}


void ASTWeaponBase::Fire()
{
	//만약 발사 딜레이가 끝났고 탄약이 있다면
	if (!bCanFire || CurrentAmmo <= 0)
	{
		if (CurrentAmmo <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("No Ammo"));
		}
		return;
	}
	
	HandleFire();
}

void ASTWeaponBase::HandleFire()
{
	if (FireSound)
	{
		//블루프린트에서 소리가 설정 되었다면
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
	
	//다음 발사를 위해 bCanFire을 비활성화 및 탄약 감소
	bCanFire = false;
	CurrentAmmo--;
	UE_LOG(LogTemp, Warning, TEXT("Bang! Ammo: %d"), CurrentAmmo);
	// 60/FireRate
	//설정 시간후 발사 활성화 함수 실행을 통해 발사 딜레이 설정
	GetWorld()->GetTimerManager().SetTimer(FireRateTimerHandle, this, &ASTWeaponBase::EnableFire, 60.0f / FireRate, false);

	// 2. 탄환 발사 (라인 트레이스)

	//화면 진동 실행
	PlayFireCameraShake();

	//이 무기를 가지고 있는 캐릭터 호출 및 컨트롤러 유무 확인후 컨트롤러 가져오기
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	
	AController* OwnerController = OwnerCharacter ? OwnerCharacter->GetController() : nullptr;
	APlayerController* PC = Cast<APlayerController>(OwnerController);

	// 1. 뷰포트(화면)의 크기를 가져와 정중앙 좌표를 구함
	int32 ViewportSizeX, ViewportSizeY;
	//화면 크기 받아오기
	PC->GetViewportSize(ViewportSizeX, ViewportSizeY);
	//화면 정중앙 지정
	const FVector2D ScreenCenter(ViewportSizeX / 2.0f, ViewportSizeY / 2.0f);

	FVector WorldLocation, WorldDirection;


	 
	//2d 화면 좌표를 현재 카메라 위치및 보는 방향으로 변환
	if (PC->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection))
	{
		//변환된 위치와 방향으로 트레이스 시작점 설정
		FVector TraceStart = WorldLocation;
		
		//라인 트레이스를 실행할 함수
		PerformTrace(TraceStart, WorldDirection);

		if (MuzzleFlashEffect)
		{
			if (ASTPlayerCharacter* PlayerCharacter = Cast<ASTPlayerCharacter>(OwnerCharacter))   // 수정 Weapon의 muzzle이 시점에 따라 변경되어야 해서 수정함 
			{
				UStaticMeshComponent* ActiveWeaponMesh =  PlayerCharacter->GetCurrentViewMode() == EViewMode::FPS ? WeaponMesh : WeaponMesh3p;

				// 방금 만든 MuzzleSocket의 위치에 총구 화염 효과를 재생합니다.
				UGameplayStatics::SpawnEmitterAttached(
					MuzzleFlashEffect,        // 재생할 파티클
					ActiveWeaponMesh,          // 부착할 컴포넌트 (총기 메시)
					TEXT("MuzzleSocket")      // 부착할 소켓 이름
				);
				PlayerCharacter->OnWeaponFired(); // 수정 캐릭터 공격시 ShootMontage(1인 위해 )
			}
			
		}
	}
}

// PerformTrace: 무기 타입에 따라 다른 방식으로 라인 트레이스 발사
void ASTWeaponBase::PerformTrace(const FVector& Start, const FVector& Direction)
{

	//충돌 검사용 변수 생성
	FCollisionQueryParams QueryParams;

	//지금 무기 및 들고있는 캐릭터 라인트레이스에서 제외
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetOwner());
	
	switch (WeaponType)
	{
	case EWeaponType::Shotgun:
		UE_LOG(LogTemp, Warning, TEXT("Shotgun firing - PelletsPerShot: %d"), PelletsPerShot);
		for (int32 i = 0; i < PelletsPerShot; ++i)
		{
			//원뿔 안에서 무작위 방향 뽑기
			FVector RandomDirection = FMath::VRandCone(Direction, FMath::DegreesToRadians(SpreadAngle));
			//라인트레이스가 끝나는 지점 구하기
			FVector TraceEnd = Start + (RandomDirection * WeaponDataAsset->WeaponData.TraceDistance);

			//충돌 결과를 담을 구조체
			FHitResult HitResult;
			//만약 맞았다면?
			if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, TraceEnd, ECC_Visibility, QueryParams))
			{
				//충돌 결과를 담을 구조체를 함수로 전달
				ProcessHit(HitResult);
			}
                
		}
		break;

	case EWeaponType::Rifle:
	case EWeaponType::Sniper:
	case EWeaponType::Pistol:
	default:
		{ // switch case 안에서 지역 변수 선언을 위해 중괄호 추가
			UE_LOG(LogTemp, Warning, TEXT("Normal gun firing..."));

			// 1. 탄 퍼짐이 적용된 최종 발사 방향 계산
			FVector FinalDirection = FMath::VRandCone(Direction, FMath::DegreesToRadians(SpreadAngle));
		
			// 2. 최종 방향으로 라인트레이스의 끝점 계산
			FVector TraceEnd = Start + (FinalDirection * WeaponDataAsset->WeaponData.TraceDistance); // TraceDistance 변수 사용

			// 3. 라인트레이스를 발사하여 실제 충돌 지점 확인
			FHitResult HitResult;
			if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, TraceEnd, ECC_Visibility, QueryParams))
			{
				TraceEnd = HitResult.ImpactPoint; // 실제 맞은 지점으로 끝점 업데이트
				ProcessHit(HitResult);
			}

			// --- [이 아래 부분이 새로 추가/수정된 트레이서 로직입니다] ---


			FVector MuzzleLocation;

			// ViewMode에 따라 분기
			ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
			if (ASTPlayerCharacter* PlayerCharacter = Cast<ASTPlayerCharacter>(OwnerCharacter))
			{
				if (PlayerCharacter->GetCurrentViewMode() == EViewMode::FPS)
				{
					MuzzleLocation = GetWeaponMesh1P()->GetSocketLocation(TEXT("MuzzleSocket"));
				}
				else // TPS
				{
					MuzzleLocation = GetWeaponMesh3P()->GetSocketLocation(TEXT("MuzzleSocket"));
				}
			}

			// ✅ 탄퍼짐이 적용된 방향으로 회전값 설정
			FRotator MuzzleRotation = FinalDirection.Rotation();  // 🎯 핵심 수정

			// 총구에서 살짝 앞쪽으로
			MuzzleLocation += FinalDirection * 20.0f;

			// --- 파티클 생성 ---
			if (TracerEffect)
			{
				UParticleSystemComponent* TracerComponent = UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					TracerEffect,
					MuzzleLocation,
					MuzzleRotation
				);

				if (TracerComponent)
				{
					TracerComponent->SetVectorParameter(FName("Target"), TraceEnd);
				}
			}
			
		}
		break;
	}
}

// ProcessHit: 충돌 결과를 받아 데미지 처리 및 디버그 드로잉
void ASTWeaponBase::ProcessHit(const FHitResult& HitResult)
{
	//에디터에서 실행시 충돌 검사용 디버그 스피어 생성
#if WITH_EDITOR
	//DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 15.f, 12, FColor::Red, false, 2.0f);
#endif

	//충돌한 액터를 가져오기
	if (AActor* HitActor = HitResult.GetActor())
	{
		//블루프린트에서 이팩트를 적용 했다면
		if (UParticleSystem* SelectedImpactEffect = ImpactEffect_Default)
		{
			// 라인트레이스가 맞은 위치에 파티클 생성
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				SelectedImpactEffect,
				HitResult.ImpactPoint,
				HitResult.ImpactNormal.Rotation()
			);
		}


		//맞은 대상에게 데미지 전달
		UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s"), *HitActor->GetName());
		AController* OwnerController = GetOwner() ? GetOwner()->GetInstigatorController() : nullptr;
		UGameplayStatics::ApplyDamage(HitActor, Damage, OwnerController, this, UDamageType::StaticClass());
	}
}


//연사 딜레이가 끝나면 호출되어 발사 가능한 상태로 만들어줌
void ASTWeaponBase::EnableFire()
{
	// bCanFire를 true로 설정하여 다시 발사할 수 있도록 허용합니다.
	bCanFire = true;
}



void ASTWeaponBase::StartReload()
{
	//장전 중 또는 탄창이 가득 찬 경우나 발사 딜레이가 호출되고 있는경우 
	if (bIsReloading || CurrentAmmo == MagazineSize || GetWorld()->GetTimerManager().IsTimerActive(FireRateTimerHandle))
	{
		return; // 세 조건 중 하나라도 참이면 재장전하지 않고 즉시 종료
	}

	//장전 사운드 실행
	if (ReloadSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, GetActorLocation());
	}
	bIsReloading = true;
	bCanFire = false;

	UE_LOG(LogTemp, Log, TEXT("Reloading..."));

	// ReloadTime 후에 FinishReload 함수 호출
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ASTWeaponBase::FinishReload, ReloadTime, false);
}

void ASTWeaponBase::FinishReload()
{
	CurrentAmmo = MagazineSize;
	bIsReloading = false;
	bCanFire = true;

	UE_LOG(LogTemp, Log, TEXT("Reload Complete. Ammo: %d"), CurrentAmmo);
}

//총기 발사 모드
EFireMode ASTWeaponBase::GetFireMode() const
{
	if (WeaponDataAsset)
	{
		return WeaponDataAsset->WeaponData.FireMode;
	}
	return EFireMode::Automatic; // 데이터 애셋이 없으면 기본값 반환
}

void ASTWeaponBase::StartFire()
{
	// 데이터 애셋에서 발사 모드를 가져옵니다.
	EFireMode Mode = GetFireMode();

	if (Mode == EFireMode::Automatic)
	{
		// 자동 연사 모드라면, 타이머를 켜서 자신의 Fire() 함수를 계속 호출합니다.
		GetWorld()->GetTimerManager().SetTimer(AutoFireTimerHandle, this, &ASTWeaponBase::Fire, 0.01f, true);
	}
	else if (Mode == EFireMode::SemiAutomatic)
	{
		// 반자동 모드라면, 자신의 Fire() 함수를 딱 한 번만 호출합니다.
		Fire();
	}
}

void ASTWeaponBase::StopFire()
{
	// 캐릭터의 마우스 버튼 떼기 신호를 받으면, 자동 연사 타이머를 멈춥니다.
	GetWorld()->GetTimerManager().ClearTimer(AutoFireTimerHandle);
}

//라이플의 경우 발사 모드 조정
void ASTWeaponBase::ToggleFireMode()
{
	// 무기 데이터가 없으면 무시
	if (!WeaponDataAsset) return;

	// 라이플 타입에서만 전환 가능하도록 제한 (원한다면 다른 무기에도 확장 가능)
	if (WeaponType != EWeaponType::Rifle)
	{
		UE_LOG(LogTemp, Warning, TEXT("이 무기는 발사 모드 전환이 불가능합니다."));
		return;
	}

	// 현재 모드를 가져옴
	EFireMode& CurrentMode = WeaponDataAsset->WeaponData.FireMode;

	// 모드를 전환
	if (CurrentMode == EFireMode::Automatic)
	{
		CurrentMode = EFireMode::SemiAutomatic;
		UE_LOG(LogTemp, Log, TEXT("발사 모드: 반자동"));
	}
	else
	{
		CurrentMode = EFireMode::Automatic;
		UE_LOG(LogTemp, Log, TEXT("발사 모드: 자동"));
	}
}

// 화면 진동 효과 재생 - 데이터 에셋의 값을 사용하도록 수정
void ASTWeaponBase::PlayFireCameraShake()
{
	// 무기를 소유한 캐릭터가 플레이어인지 확인
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return;
	}

	// 플레이어 컨트롤러 가져오기
	APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
	if (!PC)
	{
		return;
	}

	// 카메라 쉐이크 클래스가 설정되어 있는지 확인
	if (!FireCameraShake)
	{
		UE_LOG(LogTemp, Warning, TEXT("FireCameraShake not set! Please assign a camera shake class in Blueprint."));
		return;
	}

	// 데이터 에셋에서 설정된 CameraShakeScale 값을 직접 사용
	float ShakeScale = CameraShakeScale;

	// 카메라 쉐이크 재생
	PC->ClientStartCameraShake(FireCameraShake, ShakeScale);

}

//조준 시스템 구현
void ASTWeaponBase::StartAiming()
{
	bIsAiming = true;

	if (WeaponType == EWeaponType::Shotgun)
	{
		SpreadAngle = SpreadAngle/2; // 샷건은 조준해도 약간 퍼짐
	}
	else
	{
		SpreadAngle = 0; // 거의 퍼짐 없음
	}

	UE_LOG(LogTemp, Log, TEXT("Aiming... SpreadAngle: %.2f"), SpreadAngle);
}

void ASTWeaponBase::StopAiming()
{
	bIsAiming = false;
	SpreadAngle = DefaultSpreadAngle;

	UE_LOG(LogTemp, Log, TEXT("Stop Aiming. SpreadAngle restored: %.2f"), SpreadAngle);
}