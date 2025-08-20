#include "Enemy/STRangedAnimInstance.h"
#include "Enemy/STEnemyBase.h"

void USTRangedAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
    
	if (auto Enemy = Cast<ASTEnemyBase>(TryGetPawnOwner()))
	{
		Enemy->StateComponent->OnStateChanged.AddDynamic(this, &USTRangedAnimInstance::OnStateChanged);
	}
}

FString USTRangedAnimInstance::GetEnemyStateString(EEnemyState State)
{
	switch (State)
	{
	case EEnemyState::Attack:
		return TEXT("Attack");
	case EEnemyState::Idle:
		return TEXT("Idle");
	case EEnemyState::Reload:
		return TEXT("Reload");
	case EEnemyState::Dead:
		return TEXT("Dead");
	case EEnemyState::Chase:
		return TEXT("Chase");
	case EEnemyState::Investigation:
		return TEXT("Investigation");
	case EEnemyState::Patrol:
		return TEXT("Patrol");
	default:
		return TEXT("Unknown");
	}
}

void USTRangedAnimInstance::OnStateChanged(EEnemyState NewState, EEnemyState PreviousState)
{
	switch(NewState)
	{
	case EEnemyState::Attack:
		bIsReloading = false;
		bIsAttackingIn = true;
		break;
	case EEnemyState::Idle:
		bIsAttackingIn = false;
		bIsReloading = false;
		break;
	case EEnemyState::Reload:
		bIsAttackingIn = false;
		bIsReloading = true;
		break;
	case EEnemyState::Chase:
		bIsAttackingIn = false;
		break;
	case EEnemyState::Dead:
		bIsAttackingIn = false;
		bIsReloading = false;
		bIsDead = true;
		break;
	}

	// if (GEngine)
	// {
	// 	FString Msg = FString::Printf(TEXT("State Changed: %s -> %s"), 
	// 			*GetEnemyStateString(PreviousState), 
	// 			*GetEnemyStateString(NewState));
	// 	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, Msg);
	// }
}
