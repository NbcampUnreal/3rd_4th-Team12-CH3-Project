// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "STCameraComponent.generated.h"

class USpringArmComponent;

UENUM(BlueprintType)
enum class ECameraType : uint8
{
	FPS,
	TPS
};
/**
 * 
 */
UCLASS()
class ST_API USTCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	USTCameraComponent();
	void SetCameraType(ECameraType InCameraType) {CameraType = InCameraType;}
	void SetZoomFOV(float InFOV) {ZoomFOV = InFOV;}
	void SetZoomTargetArmLength(float InLength) {ZoomTargetArmLength = InLength;}
	UFUNCTION(BlueprintCallable, Category = "Camera Zoom")
	void HandleZoom(bool bIsZooming);
	
protected:
	virtual void BeginPlay() override;
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	


protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraType")
	ECameraType CameraType;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " Zoom Settings")
	float ZoomSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " Zoom Settings")
	float NormalFOV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " Zoom Settings")
	float ZoomFOV;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " TPS Settings")
	float TargetArmSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " TPS Settings")
	float NoramlTargetArmLength;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = " TPS Settings")
	float ZoomTargetArmLength;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zoom State")
	bool bIsCurrentlyZooming;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> TPSSpringArmComponent; // TPS전용
	

private:
	
	float TargetFOV;
	float TargetArmLength;
	
	bool bNeedsUpdate = false;

	void HandleFPS(float DeltaTime);
	void HandleTPS(float DeltaTime);
	

	
	
	
};
