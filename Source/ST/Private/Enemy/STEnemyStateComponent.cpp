#include "Enemy/STEnemyStateComponent.h"

#include "Enemy/STEnemyBase.h"

USTEnemyStateComponent::USTEnemyStateComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CurrentState = EEnemyState::Idle;
}

void USTEnemyStateComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USTEnemyStateComponent::SetState(EEnemyState NewState)
{
	if (CurrentState != NewState)
	{
		EEnemyState PreviousState = CurrentState;
		CurrentState = NewState;
		OnStateChanged.Broadcast(NewState, PreviousState);
	}
}

void USTEnemyStateComponent::SetPatrolMode(EPatrolMode NewMode)
{
	PatrolMode = NewMode;
}
