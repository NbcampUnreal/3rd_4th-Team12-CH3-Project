#include "UI/STDamageTextWidget.h"
#include "Components/TextBlock.h"

void USTDamageTextWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Anim_FloatUpAndFade)
	{
		FWidgetAnimationDynamicEvent FinishedEvent;
		FinishedEvent.BindDynamic(this, &USTDamageTextWidget::OnFloatUpAndFadeFinished);
		BindToAnimationFinished(Anim_FloatUpAndFade, FinishedEvent);
	}
}

void USTDamageTextWidget::OnFloatUpAndFadeFinished()
{
	RemoveFromParent();
}

void USTDamageTextWidget::SetDamageText(int32 Damage)
{
	SetDamage(Damage, false);
}

void USTDamageTextWidget::SetDamage(int32 Damage, bool bCritical)
{
	if (Txt_DamageValue)
	{
		Txt_DamageValue->SetText(FText::AsNumber(Damage));
		
		const FLinearColor UseColor = bCritical ? CriticalColor : NormalColor;
		Txt_DamageValue->SetColorAndOpacity(FSlateColor(UseColor));

		const float UseScale = bCritical ? CriticalScale : NormalScale;
		SetRenderScale(FVector2D(UseScale, UseScale));
	}

	if (Anim_FloatUpAndFade)
	{
		PlayAnimation(Anim_FloatUpAndFade);
	}
}