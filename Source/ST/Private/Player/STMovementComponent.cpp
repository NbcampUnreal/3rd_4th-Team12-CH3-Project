// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/STMovementComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Player/STPlayerCharacter.h"

// Sets default values for this component's properties
USTMovementComponent::USTMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = false;


}

void USTMovementComponent::Initialize()
{
	OwnerChar = Cast<ASTPlayerCharacter>(GetOwner());
	if (!IsValid(OwnerChar))
	{
		return;
	}
	UpdateMoveSpeed();
}

void USTMovementComponent::ChangeZoomMode()
{
	bIsZooming = !bIsZooming;
	UpdateMoveSpeed();
}

void USTMovementComponent::StartSprinting()
{
	bIsSprinting = true;
	UpdateMoveSpeed();
}

void USTMovementComponent::StopSprinting()
{
	bIsSprinting = false;
	UpdateMoveSpeed();
}


float USTMovementComponent::CalculateMoveSpeed()
{
	if (!IsValid(OwnerChar))
	{
		return 0.0f;
	}
	float FinalSpeed = WalkSpeed;
	// Sprinting?
	if (bIsSprinting)
	{
		FinalSpeed *= SprintMultiplier;
	}
	// zoom?
	if (bIsZooming)
	{
		FinalSpeed *= ZoomMultiplier;
	}

	return FinalSpeed;
}

void USTMovementComponent::UpdateMoveSpeed()
{
	if (!IsValid(OwnerChar))
	{
		return;
	}

	float NewSpeed = CalculateMoveSpeed();
	float NewCrouchSpeed = NewSpeed* CrouchMultiplier;

	if (OwnerChar)
	{
		OwnerChar->GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
		OwnerChar->GetCharacterMovement()->MaxWalkSpeedCrouched = NewCrouchSpeed;
	}
}




