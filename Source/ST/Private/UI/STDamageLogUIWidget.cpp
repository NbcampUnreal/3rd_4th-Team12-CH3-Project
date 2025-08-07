// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/STDamageLogUIWidget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "Animation/WidgetAnimation.h"


void USTDamageLogUIWidget::SetText(const FString& LogText)
{
	if (Txt_Log)
		Txt_Log->SetText(FText::FromString(LogText));
}

void USTDamageLogUIWidget::PlayFadeAndDestroy()
{
	if (FadeOutAnim)
	{
		PlayAnimation(FadeOutAnim);

		// FadeOut 후 제거 (타이머 사용)
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			[this]() { RemoveFromParent(); },
			FadeOutAnim->GetEndTime(),
			false
		);
	}
	else
	{
		RemoveFromParent();
	}
}