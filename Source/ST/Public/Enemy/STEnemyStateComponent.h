#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STEnemyStateComponent.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle			UMETA(DisplayName = "Idle"),
	Patrol			UMETA(DisplayName = "Patrol"),
	Chase			UMETA(DisplayName = "Chase"),
	Attack			UMETA(DisplayName = "Attack"),
	Reload			UMETA(DisplayName = "Reload"),
	Investigation	UMETA(DisplayName = "Investigation"),
	Dead			UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class EPatrolMode : uint8
{
	StandStill     UMETA(DisplayName="Stand Still"),
	WaypointPatrol UMETA(DisplayName="Waypoint Patrol"),
	RandomPatrol   UMETA(DisplayName="Random Patrol")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStateChanged, EEnemyState, NewState, EEnemyState, PreviousState);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ST_API USTEnemyStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:    
	USTEnemyStateComponent();

	UPROPERTY(BlueprintAssignable, Category = "Enemy|State")
	FOnStateChanged OnStateChanged;
	UFUNCTION(BlueprintPure, Category = "Enemy|State")
	EEnemyState GetCurrentState() const { return CurrentState; }

	UFUNCTION(BlueprintPure, Category = "Enemy|State")
	bool IsInState(EEnemyState State) const { return CurrentState == State; }
	UFUNCTION(BlueprintCallable, Category = "Enemy|State")
	void SetState(EEnemyState NewState);
	
	UFUNCTION(BlueprintCallable, Category = "Enemy|Patrol")
	void SetPatrolMode(EPatrolMode NewMode);
	UFUNCTION(BlueprintPure, Category = "Enemy|Patrol")
	EPatrolMode GetCurrentPatrolMode() const { return PatrolMode; }
	UFUNCTION(BlueprintPure, Category = "Enemy|Patrol")
	bool IsInPatrolMode(EPatrolMode Mode) const { return PatrolMode == Mode; }
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Enemy|State")
	EEnemyState CurrentState;

	UPROPERTY(EditAnywhere, Category = "Enemy|Patrol")
	EPatrolMode PatrolMode = EPatrolMode::RandomPatrol;
};
