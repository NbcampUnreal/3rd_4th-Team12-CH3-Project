// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/STStatusComponent.h"
#include "Engine/Engine.h"


USTStatusComponent::USTStatusComponent()
{
   
    PrimaryComponentTick.bCanEverTick = false;
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;
    WalkSpeed = 600.0f;
    SprintSpeed = 1000.0f;
    CrouchSpeed = 300.0f;
    bIsDead = false;
    bIsSprinting = false;
    bIsZooming = false;
}

void USTStatusComponent::BeginPlay()
{
    Super::BeginPlay();
 
    CurrentHealth = MaxHealth;
    
    if (WalkSpeed <= 0.0f) WalkSpeed = 600.0f;
    if (SprintSpeed <= 0.0f) SprintSpeed = 1000.0f;
    if (CrouchSpeed <= 0.0f) CrouchSpeed = 300.0f;
    if (MaxHealth <= 0.0f) 
    {
        MaxHealth = 100.0f;
        CurrentHealth = MaxHealth;
    }
}

#pragma region Health System

float USTStatusComponent::GetHealthPercentage() const
{
    if (MaxHealth <= 0.0f) return 0.0f;
    return CurrentHealth / MaxHealth;
}

void USTStatusComponent::TakeDamage(float Damage)
{
    if (bIsDead || Damage <= KINDA_SMALL_NUMBER) return;
    
    float PreviousHealth = CurrentHealth;
    CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.0f, MaxHealth);
    
    if (FMath::Abs(PreviousHealth - CurrentHealth) > KINDA_SMALL_NUMBER)
    {
        UpdateHealthStatus();
    }
}

void USTStatusComponent::Heal(float HealAmount)
{
    if (bIsDead || HealAmount <= KINDA_SMALL_NUMBER) return;
    
    float PreviousHealth = CurrentHealth;
    CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);
    
    if (FMath::Abs(PreviousHealth - CurrentHealth) > KINDA_SMALL_NUMBER)
    {
        UpdateHealthStatus();
    }
}

void USTStatusComponent::SetMaxHealth(float NewMaxHealth)
{
    if (NewMaxHealth <= 0.0f) return;
    
    float HealthRatio = MaxHealth > 0.0f ? (CurrentHealth / MaxHealth) : 1.0f;
    MaxHealth = NewMaxHealth;
    CurrentHealth = FMath::Clamp(MaxHealth * HealthRatio, 0.0f, MaxHealth);
    
    UpdateHealthStatus();
}

void USTStatusComponent::UpdateHealthStatus()
{
    if (CurrentHealth <= 0.0f && !bIsDead)
    {
        HandleDeath();
    }
    else if (!bIsDead)
    {
        OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
    }
}

void USTStatusComponent::HandleDeath()
{
    bIsDead = true;
    CurrentHealth = 0.0f;
    OnCharacterDeath.Broadcast();

}

#pragma endregion

#pragma region Movement System

float USTStatusComponent::GetMoveSpeed() const
{
    float SpeedMultiplier = 1.0f;
    if (bIsZooming)
    {
        SpeedMultiplier = 0.5f;
    }
    if (bIsDead)
    {
        SpeedMultiplier = 0.f;
    }
    
    if (bIsSprinting)
    {
        return SprintSpeed *  SpeedMultiplier;
    }
    return WalkSpeed *  SpeedMultiplier;
}

float USTStatusComponent::GetSprintSpeed() const
{
    return bIsDead ? 0.0f : SprintSpeed;
}

float USTStatusComponent::GetCrouchSpeed() const
{
    return bIsDead ? 0.0f : CrouchSpeed;
}

void USTStatusComponent::SetSprinting(bool bShouldSprint)
{
    if (bIsDead) return;
    
    bIsSprinting = bShouldSprint;
}

void USTStatusComponent::SetMoveSpeed(float NewMoveSpeed)
{
    if (NewMoveSpeed >= 0.0f)
    {
        WalkSpeed = NewMoveSpeed;
    }
}

void USTStatusComponent::SetSprintSpeed(float NewSprintSpeed)
{
    if (NewSprintSpeed >= 0.0f)
    {
        SprintSpeed = NewSprintSpeed;
    }
}

void USTStatusComponent::SetCrouchSpeed(float NewCrouchSpeed)
{
    if (NewCrouchSpeed >= 0.0f)
    {
        CrouchSpeed = NewCrouchSpeed;
    }
}

#pragma endregion