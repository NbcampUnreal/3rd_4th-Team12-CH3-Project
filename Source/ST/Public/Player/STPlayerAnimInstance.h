// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "STPlayerAnimInstance.generated.h"

enum class EWeaponType : uint8;
class ASTPlayerCharacter;
class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class ST_API USTPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	USTPlayerAnimInstance();

	void SetWeaponType(EWeaponType NewType);

protected:

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
	void UpdateMovementProperties();
	void UpdateStatusProperties();
	void CalculateAimOffset(float DeltaSeconds);
	
#pragma region Character References
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<ASTPlayerCharacter> OwnerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<UCharacterMovementComponent> OwnerCharacterMovement;

	
#pragma endregion

#pragma region Movement Properties
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float Direction;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	uint8 bShouldMove : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	uint8 bIsFalling : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	uint8 bIsCrouching : 1;
#pragma endregion

#pragma region Aim Properties
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aim")
	float AimYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aim")
	float AimPitch;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aim")
	uint8 bIsZooming : 1;
#pragma endregion

#pragma region Status Properties
protected:
	// Character Status
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	uint8 bIsDead : 1;
#pragma endregion

#pragma region Weapons Properties
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponType WeaponType;
#pragma endregion 
};
