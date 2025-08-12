// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UWCrosshairWidget.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"

void UUWCrosshairWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CrosshairLines.Empty();
	if (LineTopImage)    { CrosshairLines.Emplace(LineTopImage,    ELineDirection::Top); }        // SH
	if (LineBottomImage) { CrosshairLines.Emplace(LineBottomImage, ELineDirection::Bottom); }     // SH
	if (LineLeftImage)   { CrosshairLines.Emplace(LineLeftImage,   ELineDirection::Left); }       // SH
	if (LineRightImage)  { CrosshairLines.Emplace(LineRightImage,  ELineDirection::Right); }      // SH

	for (auto& CrosshairLine : CrosshairLines)                                                     // SH
	{
		CrosshairLine.SetZoomOffset(ZoomOffset);                                                   // SH
	}
	
	bIsInitialized =false;

	if (bHasPendingZoom)                                                                           // SH
	{
		for (auto& Line : CrosshairLines)                                                          // SH
		{
			Line.SetZooming(bDesiredZoomState);                                                    // SH
		}
		bHasPendingZoom = false;                                                                   // SH
	}
}

void UUWCrosshairWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (!bIsInitialized)
	{
		for (auto& CrosshairLine : CrosshairLines)
		{
			CrosshairLine.SetZoomOffset(ZoomOffset);
		}
		bIsInitialized = true;
		
		if (bHasPendingZoom)
		{
			for (auto& CrosshairLine : CrosshairLines)
			{
				CrosshairLine.SetZooming(bDesiredZoomState);
			}
			bHasPendingZoom = false;
		}
	}
	
	for (auto& CrosshairLine : CrosshairLines)
	{
		CrosshairLine.UpdatePosition(InDeltaTime, MoveSpeed);
	}	
}

void UUWCrosshairWidget::SetZoom(bool NewZoomMode)
{
	if (!bIsInitialized)                             // SH
	{
		bDesiredZoomState = NewZoomMode;             
		bHasPendingZoom = true;                      
		return;                                      
	}

	for (auto& CrosshairLine : CrosshairLines)
	{
		CrosshairLine.SetZooming(NewZoomMode);
	}
}

void UUWCrosshairWidget::HitEffect()
{
	if (HitAnim)
	{
		StopAnimation(HitAnim);
		PlayAnimation(HitAnim);
	}
}

#pragma region FCrosshairLine
UUWCrosshairWidget::FCrosshairLine::FCrosshairLine(UImage* InImage, ELineDirection InDirection)
	: LineImage(InImage)
	, LineDirection(InDirection)
	, DefaultPosition(FVector2D::ZeroVector)
	, MovementDirection(FVector2D::ZeroVector)
	, bIsZooming(false)
	, ZoomOffset(0.f) 
{
	if (IsValid(InImage))
	{
		LineSlot = Cast<UCanvasPanelSlot>(InImage->Slot);
		if (LineSlot)
		{
			DefaultPosition = LineSlot->GetPosition();
		}
	}

	switch (LineDirection)
	{
	case ELineDirection::Top:    MovementDirection = FVector2D(0.f, -1.f); break;
	case ELineDirection::Bottom: MovementDirection = FVector2D(0.f,  1.f); break; 
	case ELineDirection::Left:   MovementDirection = FVector2D(-1.f, 0.f); break; 
	case ELineDirection::Right:  MovementDirection = FVector2D(1.f,  0.f); break; 
	}
}

FVector2D UUWCrosshairWidget::FCrosshairLine::CalculateTotalOffset() const
{
	FVector2D TotalOffset = FVector2D::ZeroVector;
	
	if (bIsZooming)
	{
		TotalOffset += MovementDirection * ZoomOffset;
	}
	
	return TotalOffset;
}

void UUWCrosshairWidget::FCrosshairLine::UpdatePosition(float DeltaTime, float InterpSpeed)
{
	if (!IsValid(LineSlot)) return;
	
	FVector2D TargetPos = DefaultPosition + CalculateTotalOffset();
	FVector2D CurrentPos = LineSlot->GetPosition();

	if (!CurrentPos.Equals(TargetPos, 0.1f))
	{
		FVector2D NewPos = FMath::Vector2DInterpTo(CurrentPos, TargetPos, DeltaTime, InterpSpeed);
		LineSlot->SetPosition(NewPos);
	}
}


void UUWCrosshairWidget::FCrosshairLine::SetZooming(bool bZoom)
{
	bIsZooming = bZoom;
}

void UUWCrosshairWidget::FCrosshairLine::SetZoomOffset(float Offset)
{
	ZoomOffset = Offset;
}

#pragma endregion


