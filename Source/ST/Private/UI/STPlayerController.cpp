// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/STPlayerController.h"
#include "Blueprint/UserWidget.h"

void ASTPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass)
	{
		UUserWidget* HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}
}
