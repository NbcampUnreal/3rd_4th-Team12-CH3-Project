// Copyright Epic Games, Inc. All Rights Reserved.

#include "Weapon/ASTProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Kismet/KismetSystemLibrary.h" // SphereOverlapActors를 위해 추가
#include "DrawDebugHelpers.h" // DrawDebugSphere를 위해 추가

// =======================================================================
// 생성자: 이 액터가 처음 생성될 때 호출됩니다.
// =======================================================================
ASTProjectile::ASTProjectile()
{
	// 이 액터는 매 프레임 업데이트(Tick)가 필요 없으므로 비활성화합니다.
	PrimaryActorTick.bCanEverTick = false;

	// --- 컴포넌트 생성 및 설정 ---

	// 1. 충돌 컴포넌트 (USphereComponent)
	// 물리적 충돌을 감지하는 보이지 않는 구체입니다.
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;
	CollisionComponent->SetCollisionProfileName(TEXT("Projectile")); // 프로젝트 설정에 정의된 'Projectile' 콜리전 프로파일을 사용합니다.

#if WITH_EDITOR
	// 에디터에서 플레이하는 디버그 상태일 때만 충돌 범위를 보이도록 설정합니다.
	CollisionComponent->SetHiddenInGame(false);
#endif
	
	// 2. 메시 컴포넌트 (UStaticMeshComponent)
	// 플레이어의 눈에 보이는 투사체의 외형입니다.
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 시각적 외형일 뿐, 물리적 충돌은 계산하지 않습니다.

	// 3. 발사체 이동 컴포넌트 (UProjectileMovementComponent)
	// 이 컴포넌트가 투사체를 앞으로 날아가게 합니다.
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(RootComponent); // 이동의 기준은 루트 컴포넌트(CollisionComponent)입니다.
	ProjectileMovementComponent->InitialSpeed = 3000.0f;
	ProjectileMovementComponent->MaxSpeed = 3000.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true; // 날아가는 방향으로 메시가 자동으로 회전합니다.
	ProjectileMovementComponent->bShouldBounce = false;     // 벽에 닿아도 튕기지 않습니다.
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f; // 중력의 영향을 받지 않습니다.

	// --- 기본값 설정 ---

	// 맵 밖으로 영원히 날아가는 것을 방지하기 위해 5초 뒤에 자동으로 파괴됩니다.
	InitialLifeSpan = 5.0f;
	Damage = 100; // 이 값은 OnHit에서 광역 데미지가 아닌, 다른 방식으로 사용될 수 있습니다.
}

// =======================================================================
// BeginPlay: 액터가 월드에 스폰된 후, 플레이가 시작될 때 호출됩니다.
// =======================================================================
void ASTProjectile::BeginPlay()
{
	Super::BeginPlay();

	// 1. 충돌 이벤트 설정
	// CollisionComponent가 어딘가에 부딪히면(OnComponentHit), OnHit 함수를 실행하도록 연결(바인딩)합니다.
	CollisionComponent->OnComponentHit.AddDynamic(this, &ASTProjectile::OnHit);
	
	// 2. 발사자 충돌 무시 설정
	// 발사되자마자 자신을 쏜 캐릭터와 부딪히는 것을 방지합니다.
	AActor* CharacterToIgnore = GetOwner() ? GetOwner()->GetOwner() : nullptr;
	if (CharacterToIgnore)
	{
		CollisionComponent->IgnoreActorWhenMoving(CharacterToIgnore, true);
	}
}

// =======================================================================
// OnHit: CollisionComponent가 다른 물체와 부딪혔을 때 실행됩니다.
// =======================================================================
void ASTProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 이 투사체를 쏜 장본인(캐릭터)을 가져옵니다.
	AActor* ProjectileInstigator = GetInstigator();

	// 충돌한 대상이 유효하고, 자기 자신이 아니며, 자신을 쏜 캐릭터도 아닐 경우에만 아래 로직을 실행합니다.
	if (OtherActor && OtherActor != this && OtherActor != ProjectileInstigator)
	{
		// 1. 시각 효과: 충돌 지점에 폭발 이펙트를 생성합니다.
		if (ImpactEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
		}


		if (VortexEffectActorClass)
		{
			GetWorld()->SpawnActor<AActor>(VortexEffectActorClass, Hit.ImpactPoint, FRotator::ZeroRotator);
		}
		if (ImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
		}
	   
		// 2. 반복될 중력장 사운드를 생성하고, 나중에 멈출 수 있도록 변수에 저장합니다.
		if (VortexLoopSound)
		{
			VortexAudioComponent = UGameplayStatics::SpawnSoundAtLocation(this, VortexLoopSound, GetActorLocation());
		}
		// 2. 상태 변경: 투사체의 움직임을 멈추고 그 자리에 고정시킵니다.
		ProjectileMovementComponent->StopMovementImmediately();
		CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 다른 물체에 밀려나지 않도록 충돌을 끕니다.

		// 3. 타이머 설정: 중력장 효과를 시작하고, 일정 시간 뒤에 사라지도록 타이머를 설정합니다.
		GetWorld()->GetTimerManager().SetTimer(VortexTickTimerHandle, this, &ASTProjectile::ApplyVortexForce, DamageTickInterval, true);
		GetWorld()->GetTimerManager().SetTimer(VortexDurationTimerHandle, this, &ASTProjectile::StopVortexEffect, VortexDuration, false);
	}
}

// =======================================================================
// ApplyVortexForce: 중력장 효과 타이머에 의해 주기적으로 호출됩니다.
// =======================================================================
void ASTProjectile::ApplyVortexForce()
{
#if WITH_EDITOR
	// 에디터에서 플레이할 때만 보라색 구체를 그려서 중력장의 범위를 시각적으로 표시합니다.
	DrawDebugSphere(GetWorld(), GetActorLocation(), VortexRadius, 24, FColor::Purple, false, DamageTickInterval);
#endif
	
	// 1. 탐색: 투사체 주변의 일정 반경(VortexRadius) 안에 있는 모든 Pawn 타입의 액터를 찾습니다.
	TArray<AActor*> OverlappingActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), VortexRadius, ObjectTypes, nullptr, {}, OverlappingActors);

	// 2. 효과 적용: 찾은 모든 액터에 대해 반복합니다.
	for (AActor* OverlappedActor : OverlappingActors)
	{
		// 자신을 쏜 캐릭터는 효과에서 제외합니다.
		if (OverlappedActor != GetInstigator())
		{
			if (ACharacter* TargetCharacter = Cast<ACharacter>(OverlappedActor))
			{
				if (UCharacterMovementComponent* MovementComp = TargetCharacter->GetCharacterMovement())
				{
					// 적을 투사체 중심으로 끌어당기는 힘을 가합니다.
					FVector DirectionToCenter = (GetActorLocation() - TargetCharacter->GetActorLocation()).GetSafeNormal();
					MovementComp->AddImpulse(DirectionToCenter * VortexStrength, true);

					// 주기적인 데미지를 입힙니다.
					AController* OwnerController = GetOwner() ? GetOwner()->GetInstigatorController() : nullptr;
					UGameplayStatics::ApplyDamage(TargetCharacter, DamagePerTick, OwnerController, this, nullptr);
				}
			}
		}
	}
}

// =======================================================================
// StopVortexEffect: 중력장 지속시간 타이머가 끝나면 호출됩니다.
// =======================================================================
void ASTProjectile::StopVortexEffect()
{
	//사운드 정지
	if (VortexAudioComponent && VortexAudioComponent->IsPlaying())
	{
		VortexAudioComponent->Stop();
	}
	// 모든 타이머를 깨끗하게 정리하고, 이 투사체 액터를 월드에서 완전히 제거(파괴)합니다.
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Destroy();
}