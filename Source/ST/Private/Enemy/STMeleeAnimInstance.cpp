#include "Enemy/STMeleeAnimInstance.h"
#include "Enemy/STEnemyBase.h"

void USTMeleeAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
    
	if (auto Enemy = Cast<ASTEnemyBase>(TryGetPawnOwner()))
	{
		Enemy->StateComponent->OnStateChanged.AddDynamic(this, &USTMeleeAnimInstance::OnStateChanged);
	}
}

FString USTMeleeAnimInstance::GetEnemyStateString(EEnemyState State)
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

void USTMeleeAnimInstance::OnStateChanged(EEnemyState NewState, EEnemyState PreviousState)
{
	switch(NewState)
	{
	case EEnemyState::Attack:
		bIsAttackingIn = true;
		break;
	case EEnemyState::Idle:
		bIsAttackingIn = false;
		break;
	case EEnemyState::Chase:
		bIsAttackingIn = false;
		break;
	case EEnemyState::Dead:
		bIsAttackingIn = false;
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


