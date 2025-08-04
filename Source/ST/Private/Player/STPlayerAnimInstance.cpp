// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/STPlayerAnimInstance.h"
#include "Player/STPlayerCharacter.h"
#include "Player/STStatusComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

USTPlayerAnimInstance::USTPlayerAnimInstance()
{
	Velocity = FVector::Zero();
	GroundSpeed = 0.0f;
	Direction = 0.0f;
	AimYaw = 0.0f;
	AimPitch = 0.0f;
	bShouldMove = false;
	bIsFalling = false;
	bIsCrouching = false;
	bIsDead = false;
}

void USTPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	
	OwnerCharacter = Cast<ASTPlayerCharacter>(GetOwningActor());
	if (IsValid(OwnerCharacter))
	{
		OwnerCharacterMovement = OwnerCharacter->GetCharacterMovement();
	}
}

void USTPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (!IsValid(OwnerCharacter) || !IsValid(OwnerCharacterMovement))
	{
		return;
	}

	UpdateMovementProperties();
	UpdateStatusProperties();
	CalculateAimOffset(DeltaSeconds);
	
}

#pragma region Movement Properties
void USTPlayerAnimInstance::UpdateMovementProperties()
{
	if (!IsValid(OwnerCharacterMovement))
	{
		return;
	}

	
	Velocity = OwnerCharacterMovement->Velocity;
	GroundSpeed = UKismetMathLibrary::VSizeXY(Velocity);
	bIsFalling = OwnerCharacterMovement->IsFalling();
	bIsCrouching = OwnerCharacterMovement->IsCrouching();

	
	if (Velocity.SizeSquared() > 0.0f)
	{
		FVector LocalVelocity = OwnerCharacter->GetActorTransform().InverseTransformVectorNoScale(Velocity);
		Direction = FMath::RadiansToDegrees(FMath::Atan2(LocalVelocity.Y, LocalVelocity.X));
	}
	
	float GroundAcceleration = UKismetMathLibrary::VSizeXY(OwnerCharacterMovement->GetCurrentAcceleration());
	bool bIsAccelerated = !FMath::IsNearlyZero(GroundAcceleration);
	bShouldMove = (GroundSpeed > KINDA_SMALL_NUMBER) && bIsAccelerated;
}
#pragma endregion

#pragma region Aim Properties
void USTPlayerAnimInstance::CalculateAimOffset(float DeltaSeconds)
{
	if (!IsValid(OwnerCharacter))
	{
		return;
	}

	
	FRotator ControlRotation = OwnerCharacter->GetControlRotation();
	FRotator ActorRotation = OwnerCharacter->GetActorRotation();
	FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(ControlRotation, ActorRotation);


	FRotator TargetAimOffset = FRotator(DeltaRotation.Pitch, DeltaRotation.Yaw, 0.0f);
	FRotator CurrentAimOffset = FRotator(AimPitch, AimYaw, 0.0f);


	FRotator InterpolatedAimOffset = FMath::RInterpTo(CurrentAimOffset, TargetAimOffset, DeltaSeconds, 15.0f);


	AimYaw = FMath::Clamp(InterpolatedAimOffset.Yaw, -90.0f, 90.0f);
	AimPitch = FMath::Clamp(InterpolatedAimOffset.Pitch, -90.0f, 90.0f);
}
#pragma endregion

#pragma region Status Properties
void USTPlayerAnimInstance::UpdateStatusProperties()
{
	if (!IsValid(OwnerCharacter))
	{
		bIsDead = false;
		return;
	}

	USTStatusComponent* CurrentStatusComponent = OwnerCharacter->GetStatusComponent();
	if (IsValid(CurrentStatusComponent))
	{
		bIsDead = CurrentStatusComponent->IsDead();
	}
	else
	{
		bIsDead = false;
	}
}
#pragma endregion


