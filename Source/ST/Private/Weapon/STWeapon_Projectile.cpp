// Copyright Epic Games, Inc. All Rights Reserved.

#include "Weapon/STWeapon_Projectile.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/ASTProjectile.h"
#include "Player/STPlayerCharacter.h"

void ASTWeapon_Projectile::FireWeapon()
{
	// =======================================================================
	// 1. 안전 점검 (Prerequisites Check)
	// =======================================================================
	// 발사에 필요한 기본 조건들이 갖춰졌는지 순서대로 확인합니다.

	// 무기를 소유하고 있는 캐릭터가 있는지 확인합니다.
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter) return;

	// 블루프린트에서 발사할 투사체 클래스가 지정되었는지 확인합니다.
	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("ProjectileClass is not set in the Blueprint for %s!"), *GetName());
		return;
	}

	// 캐릭터를 조종하는 플레이어 컨트롤러가 있는지 확인합니다.
	AController* OwnerController = OwnerCharacter->GetController();
	if (!OwnerController)
	{
		return;
	}
	
	// =======================================================================
	// 2. 발사 위치 및 방향 계산 (Location & Rotation Calculation)
	// =======================================================================
	
	FVector MuzzleLocation;
	
	// 소유자 캐릭터가 1인칭/3인칭 전환 기능을 가진 STPlayerCharacter인지 확인합니다.
	if (ASTPlayerCharacter* PlayerCharacter = Cast<ASTPlayerCharacter>(OwnerCharacter))
	{
		// 현재 뷰 모드에 따라 사용할 무기 메시를 결정합니다.
		UStaticMeshComponent* ActiveWeaponMesh = PlayerCharacter->GetCurrentViewMode() == EViewMode::FPS ? GetWeaponMesh1P() : GetWeaponMesh1P();
		
		// 선택된 메시에서 총구(MuzzleSocket)의 월드 좌표를 가져옵니다.
		MuzzleLocation = ActiveWeaponMesh->GetSocketLocation(TEXT("MuzzleSocket"));
	}
	else
	{
		// 만약 다른 종류의 캐릭터라면, 기본적으로 1인칭 메시에서 위치를 가져옵니다.
		MuzzleLocation = GetWeaponMesh1P()->GetSocketLocation(TEXT("MuzzleSocket"));
	}
	
	// 플레이어의 카메라가 바라보는 방향을 투사체의 발사 방향으로 설정합니다.
	FRotator MuzzleRotation = OwnerController->GetControlRotation();

	// =======================================================================
	// 3. 생성 규칙 설정 (Spawn Parameters Setup)
	// =======================================================================
	// 투사체를 생성할 때 적용할 세부 규칙들을 설정합니다.
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this; // 투사체의 소유자(Owner)는 이 무기 액터 자신입니다.
	SpawnParams.Instigator = OwnerCharacter->GetInstigator(); // 행동의 책임자(Instigator)는 플레이어 캐릭터입니다.
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; // 충돌과 관계없이 항상 생성합니다.


	// =======================================================================
	// 4. 최종 생성 위치 계산 (Final Spawn Location with Offset)
	// =======================================================================
	// 캐릭터와의 충돌을 피하기 위해, 계산된 총구 위치에서 약간 앞쪽으로 떨어진 위치를 최종 생성 지점으로 정합니다.

	// 발사 방향을 벡터로 변환합니다.
	FVector ForwardDirection = MuzzleRotation.Vector();

	// 기존 총구 위치에 오프셋(SpawnOffset)을 더해 최종 생성 위치를 계산합니다.
	FVector SpawnLocation = MuzzleLocation + (ForwardDirection * SpawnOffset);
	
	// =======================================================================
	// 5. 투사체 생성 (Spawn Projectile)
	// =======================================================================
	// 준비된 모든 정보를 바탕으로 월드에 투사체를 생성(스폰)합니다.
	
	GetWorld()->SpawnActor<ASTProjectile>(ProjectileClass, SpawnLocation, MuzzleRotation, SpawnParams);
}