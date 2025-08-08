// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STDamageLogUIWidget.generated.h"

class UTextBlock;
class UWidgetAnimation;

UCLASS()
class ST_API USTDamageLogUIWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetText(const FString& LogText);
	void PlayFadeAndDestroy();

protected:
	UPROPERTY(meta = (BindWidget)) UTextBlock* Txt_Log;

	UPROPERTY(meta = (BindWidgetAnim), Transient) UWidgetAnimation* FadeOutAnim;
};
