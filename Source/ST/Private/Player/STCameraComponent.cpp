// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/STCameraComponent.h"

#include "GameFramework/SpringArmComponent.h"

USTCameraComponent::USTCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	// Zoom Settings 
	ZoomSpeed = 5.0f;
	NormalFOV = 90.0f;
	ZoomFOV = 60.0f;
   
	// TPS Settings 
	TargetArmSpeed = 3.0f;
	NoramlTargetArmLength = 300.0f; 
	ZoomTargetArmLength = 150.0f;
	bIsCurrentlyZooming = false;
	
	bNeedsUpdate = false;
}

void USTCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	NormalFOV = FieldOfView;
	if (CameraType == ECameraType::TPS)
	{
		TPSSpringArmComponent = Cast<USpringArmComponent>(GetAttachParent());
		if (TPSSpringArmComponent)  
		{
			TPSSpringArmComponent->TargetArmLength = NoramlTargetArmLength;
		}
	}
}

void USTCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bNeedsUpdate)return;

	if (CameraType == ECameraType::TPS)
	{
		HandleTPS(DeltaTime);
	}
	else
	{
		HandleFPS(DeltaTime);
	}
	

}

void USTCameraComponent::HandleZoom(bool bIsZooming)
{
	bIsCurrentlyZooming =  bIsZooming;

	if (CameraType == ECameraType::TPS)
	{
		if (bIsCurrentlyZooming)
		{
			TargetFOV = ZoomFOV;
			TargetArmLength = ZoomTargetArmLength;
		}
		else
		{
			TargetFOV = NormalFOV;
			TargetArmLength = NoramlTargetArmLength;
		}
	}
	else
	{
		if (bIsCurrentlyZooming)
		{
			TargetFOV = ZoomFOV;
		}
		else
		{
			TargetFOV = NormalFOV;
			
		}
	}
	bNeedsUpdate =true;
}

void USTCameraComponent::HandleFPS(float DeltaTime)
{
	float CurrentFOV = FieldOfView;  
	float NewFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, ZoomSpeed);
	SetFieldOfView(NewFOV);
    
	if ((NewFOV - TargetFOV) <= KINDA_SMALL_NUMBER)  // 절댓값 사용
	{
		SetFieldOfView(TargetFOV);
		bNeedsUpdate = false;
	}
}

void USTCameraComponent::HandleTPS(float DeltaTime)
{
	float CurrentArmLength = TPSSpringArmComponent->TargetArmLength;
	float CurrentFOV = FieldOfView;  
    
	float NewArmLength = FMath::FInterpTo(CurrentArmLength, TargetArmLength, DeltaTime, TargetArmSpeed);
	TPSSpringArmComponent->TargetArmLength = NewArmLength;
    
	float NewFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, ZoomSpeed);
	SetFieldOfView(NewFOV);
    
	
	if ((NewArmLength - TargetArmLength) <= KINDA_SMALL_NUMBER && 
		(NewFOV - TargetFOV) <= KINDA_SMALL_NUMBER)
	{
		TPSSpringArmComponent->TargetArmLength = TargetArmLength;
		SetFieldOfView(TargetFOV);
		bNeedsUpdate = false;
	}
}

