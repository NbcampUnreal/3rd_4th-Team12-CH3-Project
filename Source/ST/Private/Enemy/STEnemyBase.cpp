#include "Enemy/STEnemyBase.h"
#include "Enemy/STEnemyAlertReceiver.h"
#include "Engine/DamageEvents.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "System/STGameMode.h"

ASTEnemyBase::ASTEnemyBase()
{
 	PrimaryActorTick.bCanEverTick = false;

	// 상태 관리 컴포넌트
	StateComponent = CreateDefaultSubobject<USTEnemyStateComponent>(TEXT("StateComponent"));

	// 약점 부위와 피해 배율 설정
	WeakPointMultipliers.Add(FName("head"), 2.0f);
	WeakPointMultipliers.Add(FName("back"), 1.5f);
}

void ASTEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	// 상대가 바뀔 때 적 이동 속도 업데이트
	if (StateComponent)
	{
		StateComponent->OnStateChanged.AddDynamic(this, &ASTEnemyBase::OnStateChanged_UpdateSpeed);
	}
}

float ASTEnemyBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if (StateComponent->GetCurrentState() == EEnemyState::Dead)
	{
		return 0.0f;
	}
	
	// 적이 피격 당했을 때 인터페이스에서 가져온 NotifyPlayerAttack를 호출하고
	// 해당 AIController에서 인지 게이지 증가
	if (ISTEnemyAlertReceiver* AlertReceiver = Cast<ISTEnemyAlertReceiver>(GetController()))
	{
		AlertReceiver->NotifyPlayerAttack(this);
	}
	
	FName HitBone=NAME_None;
	FVector HitLocation=GetActorLocation();

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PDmg= static_cast<const FPointDamageEvent*>(&DamageEvent);
		HitBone=PDmg->HitInfo.BoneName;
		HitLocation=PDmg->HitInfo.ImpactPoint;
	}

	// 약점 배율 + 적 방어력 계산 후 실제 데미지 적용
	ProcessDamage(DamageAmount, HitBone, HitLocation);
	
	return DamageAmount;
}

void ASTEnemyBase::ProcessDamage(float RawDamage, FName HitBone, FVector HitLocation)
{
	if (StateComponent->GetCurrentState() == EEnemyState::Dead)
	{
		return;
	}
	float Multiplier=1.0f;
	if (WeakPointMultipliers.Contains(HitBone))
	{
		Multiplier=WeakPointMultipliers[HitBone];
	}
	
	// 약점 배율 + 적 방어력 계산 후 실제 데미지 적용
	float TrueDamage=FMath::Max(0.0f, RawDamage*Multiplier-Defense);
	Health=FMath::Max(0.0f, Health-TrueDamage);

	if (Health<=0 && !StateComponent->IsInState(EEnemyState::Dead))
	{
		Die();
	}
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			6.0f,
			FColor::Green,
			*FString::Printf(TEXT("Damage: %0.f, Health: %0.f"), TrueDamage, Health)
			);
	}
}

void ASTEnemyBase::Die()
{
	if (StateComponent->GetCurrentState() == EEnemyState::Dead)
	{
		return;
	}
	// State를 Dead로 변경
	StateComponent->SetState(EEnemyState::Dead);

	// 게임 모드에 OnEnemyKilled 호출
	if (UWorld* World = GetWorld())
	{
		if (auto GameMode = Cast<ASTGameMode>(World->GetAuthGameMode()))
		{
			GameMode->OnEnemyKilled();
		}
	}

	// 4초 후 시체 소멸
	SetLifeSpan(4.0f);
}

void ASTEnemyBase::OnStateChanged_UpdateSpeed(EEnemyState NewState, EEnemyState PrevState)
{
	UpdateSpeedByState(NewState);
}

void ASTEnemyBase::UpdateSpeedByState(EEnemyState NewState)
{
	float TargetSpeed = PatrolSpeed;

	switch (NewState)
	{
	case EEnemyState::Patrol:
		TargetSpeed = PatrolSpeed;
		break;
	case EEnemyState::Investigation:
		TargetSpeed = InvestigationSpeed;
		break;
	case EEnemyState::Chase:
		TargetSpeed = ChaseSpeed;
		break;
	default:
		TargetSpeed = PatrolSpeed;
	}

	GetCharacterMovement()->MaxWalkSpeed = TargetSpeed;
}




