// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/STPlayerAnimInstance.h"
#include "Player/STPlayerCharacter.h"
#include "Player/STStatusComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapon/STWeaponType.h"

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
	bIsZooming = false;
	WeaponType = EWeaponType::Rifle;
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
	bIsZooming = OwnerCharacter->GetStatusComponent()->IsZooming();
}
#pragma endregion

#pragma region Aim Properties
void USTPlayerAnimInstance::CalculateAimOffset(float DeltaSeconds)
{
	FRotator ControlRotation = OwnerCharacter->GetControlRotation();
	FRotator ActorRotation = OwnerCharacter->GetActorRotation();

	float TargetYaw = FMath::UnwindDegrees(ControlRotation.Yaw - ActorRotation.Yaw);
	float TargetPitch = FMath::UnwindDegrees(ControlRotation.Pitch - ActorRotation.Pitch);

	FRotator TargetAimOffset(TargetPitch, TargetYaw, 0.f);
	FRotator CurrentAimOffset(AimPitch, AimYaw, 0.f);

	float InterpSpeed = bIsZooming ? 8.f : 20.f;
	if (bShouldMove) InterpSpeed *= 1.5f;

	FRotator Interpolated = FMath::RInterpTo(CurrentAimOffset, TargetAimOffset, DeltaSeconds, InterpSpeed);

	// Clamp after Interpolation
	AimYaw = FMath::Clamp(Interpolated.Yaw, -90, 90);
	AimPitch = FMath::Clamp(Interpolated.Pitch, -90, 90);

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

#pragma region WeaponProperties
void USTPlayerAnimInstance::SetWeaponType(EWeaponType NewType)
{
	if (WeaponType == NewType)
	{
		return;
	}
	WeaponType = NewType;
}
#pragma endregion