#include "Enemy/STBossAnimInstance.h"
#include "Enemy/STEnemyBase.h"

void USTBossAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	if (auto* Enemy = Cast<ASTEnemyBase>(TryGetPawnOwner()))
	{
		if (Enemy->StateComponent)
		{
			Enemy->StateComponent->OnStateChanged.AddDynamic(this, &USTBossAnimInstance::OnStateChanged);
		}
	}
}

FString USTBossAnimInstance::GetEnemyStateString(EEnemyState State)
{
	switch (State)
	{
	case EEnemyState::Idle:           return TEXT("Idle");
	case EEnemyState::Patrol:         return TEXT("Patrol");
	case EEnemyState::Chase:          return TEXT("Chase");
	case EEnemyState::Attack:         return TEXT("Attack");
	case EEnemyState::Investigation:  return TEXT("Investigation");
	case EEnemyState::PhaseChanging:  return TEXT("PhaseChanging");
	case EEnemyState::Dead:           return TEXT("Dead");
	default:                          return TEXT("Unknown");
	}
}

void USTBossAnimInstance::OnStateChanged(EEnemyState NewState, EEnemyState PreviousState)
{
	bIsIdle           = false;
	bIsPatrol         = false;
	bIsChase          = false;
	bIsAttack         = false;
	bIsInvestigating  = false;
	bIsDead           = false;
	bIsPhaseChangingStart  = false;
	bIsPhaseChangingEnd  = false;

	switch (NewState)
	{
	case EEnemyState::Idle:           bIsIdle = true; break;
	case EEnemyState::Patrol:         bIsPatrol = true; break;
	case EEnemyState::Chase:          bIsChase = true; break;
	case EEnemyState::Attack:         bIsAttack = true; break;
	case EEnemyState::Investigation:  bIsInvestigating = true; break;
	case EEnemyState::PhaseChanging:  bIsPhaseChangingStart = true; break;
	case EEnemyState::Dead:           bIsDead = true; break;
	default: break;
	}

	// if (GEngine)
	// {
	// 	FString PreviousStateString = GetEnemyStateString(PreviousState);
	// 	FString NewStateString = GetEnemyStateString(NewState);
	// 	
	// 	FColor LogColor = FColor::White;
	// 	switch (NewState)
	// 	{
	// 	case EEnemyState::Idle:           LogColor = FColor::Green; break;
	// 	case EEnemyState::Attack:         LogColor = FColor::Red; break;
	// 	case EEnemyState::PhaseChanging:  LogColor = FColor::Magenta; break;
	// 	case EEnemyState::Chase:          LogColor = FColor::Yellow; break;
	// 	case EEnemyState::Dead:           LogColor = FColor::Black; break;
	// 	default:                          LogColor = FColor::White; break;
	// 	}
	// 	
	// 	GEngine->AddOnScreenDebugMessage(-1, 3.f, LogColor,
	// 		FString::Printf(TEXT("[BOSS STATE CHANGE] %s → %s"), 
	// 			*PreviousStateString, *NewStateString));
	// }
}
