#include "UI/STDamageTextWidget.h"
#include "Components/TextBlock.h"

void USTDamageTextWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void USTDamageTextWidget::SetDamageText(int32 Damage)
{
	if (Txt_DamageValue)
	{
		Txt_DamageValue->SetText(FText::AsNumber(Damage));
		Txt_DamageValue->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	}

	if (Anim_FloatUpAndFade)
	{
		PlayAnimation(Anim_FloatUpAndFade);
	}
}