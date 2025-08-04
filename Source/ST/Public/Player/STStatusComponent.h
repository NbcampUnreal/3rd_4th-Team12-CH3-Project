// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STStatusComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, CurrentHP, float, MaxHP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDeath);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ST_API USTStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    // Sets default values for this component's properties
    USTStatusComponent();

    // Called when the game starts
    virtual void BeginPlay() override;

#pragma region Health System
public:
    // Health getters
    UFUNCTION(BlueprintPure, Category = "Health")
    bool IsDead() const { return bIsDead; }

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetCurrentHealth() const { return CurrentHealth; }

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetMaxHealth() const { return MaxHealth; }

    UFUNCTION(BlueprintPure, Category = "Health")
    float GetHealthPercentage() const;

    // Health manipulation
    UFUNCTION(BlueprintCallable, Category = "Health")
    void TakeDamage(float Damage);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void Heal(float HealAmount);

    UFUNCTION(BlueprintCallable, Category = "Health")
    void SetMaxHealth(float NewMaxHealth);

    // Delegates
    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnHealthChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnCharacterDeath OnCharacterDeath;

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (ClampMin = "1.0"))
    float MaxHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
    float CurrentHealth = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
    bool bIsDead = false;

private:
    void UpdateHealthStatus();
    void HandleDeath();

#pragma endregion

#pragma region Movement System
public:
    // Movement getters
    UFUNCTION(BlueprintPure, Category = "Movement")
    float GetMoveSpeed() const;

    UFUNCTION(BlueprintPure, Category = "Movement")
    float GetSprintSpeed() const;

    UFUNCTION(BlueprintPure, Category = "Movement")
    float GetCrouchSpeed() const;

    UFUNCTION(BlueprintPure, Category = "Movement")
    bool IsSprinting() const { return bIsSprinting; }

    // Movement setters
    UFUNCTION(BlueprintCallable, Category = "Movement")
    void SetSprinting(bool bShouldSprint);

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void SetMoveSpeed(float NewMoveSpeed);

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void SetSprintSpeed(float NewSprintSpeed);

    UFUNCTION(BlueprintCallable, Category = "Movement")
    void SetCrouchSpeed(float NewCrouchSpeed);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.0"))
    float WalkSpeed = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.0"))
    float SprintSpeed = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.0"))
    float CrouchSpeed = 300.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    bool bIsSprinting = false;

#pragma endregion

#pragma region Zoom

public:
    void SetZoom(bool bShouldZoom) { bIsZooming = bShouldZoom; }
    bool IsZooming() const { return bIsZooming; }
protected:
    uint8 bIsZooming : 1 = false;

#pragma endregion 
};
