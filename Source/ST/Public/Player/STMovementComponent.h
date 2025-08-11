// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STMovementComponent.generated.h"


class ASTPlayerCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ST_API USTMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	USTMovementComponent();

	void Initialize();

public:
	
	bool IsZooming() const { return bIsZooming; }

	void SetWalkSpeed(float InWalkSpeed) { WalkSpeed = InWalkSpeed; }
	void SetSprintMultiplier(float InSprintMultiplier){ SprintMultiplier = InSprintMultiplier; }
	void SetCrouchMultiplier(float InCrouchMultiplier){ CrouchMultiplier = InCrouchMultiplier; }
	void SetZoomMultiplier(float InZoomMultiplier){ ZoomMultiplier = InZoomMultiplier; }
	
	void ChangeZoomMode();

	void StartSprinting();
	
	void StopSprinting();



protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.0"))
	float WalkSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.0", ClampMax = "2.0"))
	float SprintMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float CrouchMultiplier = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ZoomMultiplier = 0.3f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsSprinting = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsCrouch = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsZooming = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Owner")
	TObjectPtr<ASTPlayerCharacter> OwnerChar;

private:
	float CalculateMoveSpeed();
	void UpdateMoveSpeed();
};
