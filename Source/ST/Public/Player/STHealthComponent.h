// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, CurrentHP, float, MaxHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDeath);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ST_API USTHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USTHealthComponent();

public:
	
	bool IsDead() const { return bIsDead; }
	
	float GetCurrentHealth() const { return CurrentHealth; }
	
	float GetMaxHealth() const { return MaxHealth; }
	
	void TakeDamage(float Damage);
	
	void Heal(float HealAmount);
	
	void SetMaxHealth(float NewMaxHealth);

	void Initialize();

	// Delegates
	FOnHealthChanged OnHealthChanged;
	
	FOnCharacterDeath OnCharacterDeath;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	bool bIsDead = false;

private:
	void UpdateHealthStatus(); // 캐릭터 상태 체크
	
};
