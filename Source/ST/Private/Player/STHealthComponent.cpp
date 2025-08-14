// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/STHealthComponent.h"

// Sets default values for this component's properties
USTHealthComponent::USTHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void USTHealthComponent::Initialize()
{
	CurrentHealth = MaxHealth;
	UpdateHealthStatus();
}

void USTHealthComponent::TakeDamage(float Damage)
{
	if (bIsDead) return;
	
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);

	// SH 데미지 델리게이트 호출
	if (OnHealthDamage.IsBound())
	{
		OnHealthDamage.Broadcast(Damage);
	}

	UpdateHealthStatus();
}

void USTHealthComponent::Heal(float HealAmount)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.f, MaxHealth);

	// SH 회복 델리게이트 호출
	if (OnHealthHeal.IsBound())
	{
		OnHealthHeal.Broadcast(HealAmount);
	}
	
	UpdateHealthStatus();
}

void USTHealthComponent::SetMaxHealth(float NewMaxHealth)
{
	MaxHealth = NewMaxHealth;
}

void USTHealthComponent::SetCurrentHealth(float NewCurrentHealth)
{
	CurrentHealth = NewCurrentHealth;
}


/* void USTHealthComponent::UpdateHealthStatus()
{
	if (CurrentHealth < KINDA_SMALL_NUMBER)
	{
		bIsDead = true;
		if (OnCharacterDeath.IsBound())
		{
			OnCharacterDeath.Broadcast();
		}
	}
	else
	{
		if (OnHealthChanged.IsBound())
		{
			OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
		}
	}
} */


void USTHealthComponent::UpdateHealthStatus()
{
	if (OnHealthChanged.IsBound())
	{
		OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	}

	const bool bNowDead = (CurrentHealth <= KINDA_SMALL_NUMBER);
	if (bNowDead && !bIsDead)
	{
		bIsDead = true;
		if (OnCharacterDeath.IsBound())
		{
			OnCharacterDeath.Broadcast();
		}
	}
}




