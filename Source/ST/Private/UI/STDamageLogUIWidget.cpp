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
	/*if (FadeOutAnim)
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
	}*/

	// JM: 기존 위의 방식대로면 레벨이동시 캡쳐한 this가 사라지면서 잘못된 접근이 발생함.
	// 아래와 같이 수정 함.
	if (FadeOutAnim)
	{
		PlayAnimation(FadeOutAnim);

		// FadeOut 후 제거 (타이머 사용)
		FTimerHandle TimerHandle;
		TWeakObjectPtr<USTDamageLogUIWidget> WeakThis(this);	// JM: Weak_Ptr로 변경
		
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			[WeakThis]()
			{
				if (WeakThis.IsValid())
				{
					WeakThis->RemoveFromParent();
				}
			},
			FadeOutAnim->GetEndTime(),
			false
		);
	}
	else
	{
		RemoveFromParent();
	}
}