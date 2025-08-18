// Copyright Epic Games, Inc. All Rights Reserved.

#include "Weapon/STWeapon_Hitscan.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Player/STPlayerCharacter.h" // 캐릭터 클래스 포함

// 발사 실제 로직 구현
void ASTWeapon_Hitscan::FireWeapon()
{
	//캐릭터 가져오기 및 캐릭터 타입인지 확인
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter) return;
	//컨트롤러 가져오기
	AController* OwnerController = OwnerCharacter->GetController();
	APlayerController* PC = Cast<APlayerController>(OwnerController);
	if (!PC) return;

	//화면 중앙에서 월드 방향을 계산합니다.
	int32 ViewportSizeX, ViewportSizeY;
	PC->GetViewportSize(ViewportSizeX, ViewportSizeY);
	const FVector2D ScreenCenter(ViewportSizeX / 2.0f, ViewportSizeY / 2.0f);

	FVector WorldLocation, WorldDirection;
	if (PC->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection))
	{
		// 2. 계산된 위치와 방향으로 라인 트레이스를 발사합니다.
		PerformTrace(WorldLocation, WorldDirection);
	}
}


void ASTWeapon_Hitscan::PerformTrace(const FVector& Start, const FVector& Direction)
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

	case EWeaponType::Rifle:   // falls through
	case EWeaponType::Sniper:  // falls through
	case EWeaponType::Pistol:  // falls through
	default:
		{ 
			UE_LOG(LogTemp, Warning, TEXT("Normal gun firing..."));

			// 1. 탄 퍼짐이 적용된 최종 발사 방향 계산
			FVector FinalDirection = FMath::VRandCone(Direction, FMath::DegreesToRadians(SpreadAngle));

			// 2. 최종 방향으로 라인트레이스의 끝점 계산
			FVector TraceEnd = Start + (FinalDirection * WeaponDataAsset->WeaponData.TraceDistance);
			DrawDebugLine(GetWorld(), Start, TraceEnd, FColor::Green, false, 2.0f);

			// 3. 라인트레이스를 발사하여 실제 충돌 지점 확인
			FHitResult HitResult;
			if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, TraceEnd, ECC_Visibility, QueryParams))
			{
				TraceEnd = HitResult.ImpactPoint; 
				ProcessHit(HitResult);
			}

			// --- [트레이서 로직] ---
			if (TracerEffect)
			{
				ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
				if (ASTPlayerCharacter* PlayerCharacter = Cast<ASTPlayerCharacter>(OwnerCharacter))
				{
                    // 총구 위치 가져오기
					UStaticMeshComponent* ActiveWeaponMesh = PlayerCharacter->GetCurrentViewMode() == EViewMode::FPS ? GetWeaponMesh1P() : GetWeaponMesh3P();
					FVector MuzzleLocation = ActiveWeaponMesh->GetSocketLocation(TEXT("MuzzleSocket"));
					
                    // 시차 보정을 위한 방향 재계산
                    FVector CorrectedDirection = (TraceEnd - MuzzleLocation).GetSafeNormal();
                    FRotator CorrectedRotation = CorrectedDirection.Rotation();

					UParticleSystemComponent* TracerComponent = UGameplayStatics::SpawnEmitterAtLocation(
						GetWorld(),
						TracerEffect,
						MuzzleLocation,
						CorrectedRotation // 보정된 회전값 사용
					);

					if (TracerComponent)
					{
						TracerComponent->SetVectorParameter(FName("Target"), TraceEnd);
					}
				}
			}
		}
		break;
	}
}


void ASTWeapon_Hitscan::ProcessHit(const FHitResult& HitResult)
{
	if (AActor* HitActor = HitResult.GetActor())
	{
		// 피격 이펙트 재생 (ImpactEffect_Default는 부모 클래스에 있으므로 접근 가능)
		if (ImpactEffect_Default)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				ImpactEffect_Default,
				HitResult.ImpactPoint,
				HitResult.ImpactNormal.Rotation()
			);
		}

		// 데미지 처리
		AController* OwnerController = GetOwner() ? GetOwner()->GetInstigatorController() : nullptr;

		UGameplayStatics::ApplyPointDamage
		(
			HitActor,                      
			Damage, // Damage 변수도 부모 클래스에 있으므로 접근 가능
			HitResult.ImpactPoint,         
			HitResult,                     
			OwnerController,               
			this,                          
			UDamageType::StaticClass()     
		);
	}
}