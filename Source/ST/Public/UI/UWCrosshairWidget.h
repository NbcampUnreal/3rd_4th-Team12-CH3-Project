// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UWCrosshairWidget.generated.h"

class UCanvasPanelSlot;
class UImage;
/**
 * 
 */
UCLASS()
class ST_API UUWCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetZoom(bool NewZoomMode);

	UFUNCTION(BlueprintCallable)
	void HitEffect();
	
#pragma region FCrosshairLine
public:
	enum class ELineDirection
	{
		Top,
		Bottom,
		Left,
		Right
	};
	
	class FCrosshairLine
	{
	public:
		FCrosshairLine(UImage* InImage , ELineDirection InDirection);
		FVector2D CalculateTotalOffset() const;
		void UpdatePosition(float DeltaTime, float InterpSpeed);
		void SetZooming(bool inZooming);
		void SetZoomOffset(float Offset);

	private:
		//properties 
		UImage* LineImage;
		UCanvasPanelSlot* LineSlot;
		FVector2D DefaultPosition;
		ELineDirection LineDirection;
		FVector2D MovementDirection;

		//zoom
		bool bIsZooming;
		float ZoomOffset;
		
	};
#pragma endregion  
protected:
	
	UPROPERTY(meta = (BindWidget))
	UImage* LineTopImage;

	UPROPERTY(meta = (BindWidget))
	UImage* LineBottomImage;

	UPROPERTY(meta = (BindWidget))
	UImage* LineLeftImage;

	UPROPERTY(meta = (BindWidget))
	UImage* LineRightImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair|Offset")
	float ZoomOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair|Offset")
	float SpreadOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair|Speed")
	float MoveSpeed;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> HitAnim;

private:
	TArray<FCrosshairLine> CrosshairLines;
	
	bool bIsInitialized;
	
	
	
};
