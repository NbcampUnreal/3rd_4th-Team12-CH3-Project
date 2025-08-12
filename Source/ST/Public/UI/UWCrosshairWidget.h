// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UWCrosshairWidget.generated.h"

class UCanvasPanelSlot;
class UImage;


UCLASS()
class ST_API UUWCrosshairWidget : public UUserWidget
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable)
	void SetZoom(bool NewZoomMode);

	UFUNCTION(BlueprintCallable)
	void HitEffect();
	
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY(meta = (BindWidget)) UImage* LineTopImage;
	UPROPERTY(meta = (BindWidget)) UImage* LineBottomImage;
	UPROPERTY(meta = (BindWidget)) UImage* LineLeftImage;
	UPROPERTY(meta = (BindWidget)) UImage* LineRightImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair|Offset")
	float ZoomOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair|Offset")
	float SpreadOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair|Speed")
	float MoveSpeed;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> HitAnim;

	

private:
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
		bool bIsZooming = false;   // SH: 기본값 초기화
		float ZoomOffset = 0.f;    // SH: 기본값 초기화
		
	};
	
	TArray<FCrosshairLine> CrosshairLines;

	bool bIsInitialized = false;;        // SH: 초기화 완료 플래그
	bool bDesiredZoomState = false;      // SH: 초기화 전에 전달된 줌 상태 저장
	bool bHasPendingZoom = false;        // SH: 초기화 후 1회 적용 플래그
	
	
	
};
