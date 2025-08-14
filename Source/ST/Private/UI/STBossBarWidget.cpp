// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/STBossBarWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void USTBossBarWidget::SetBossName(const FText& InName)
{
	if (Txt_BossName)
	{
		Txt_BossName->SetText(InName);
	}
}

void USTBossBarWidget::UpdateBossHP(float CurrentHP, float MaxHP)
{
	const float SafeMax = FMath::Max(1.f, MaxHP);
	const float Ratio   = FMath::Clamp(CurrentHP / SafeMax, 0.f, 1.f);

	if (PB_BossHealth)
	{
		PB_BossHealth->SetPercent(Ratio);

		const FLinearColor NewColor = ComputeHPColor(Ratio);
		PB_BossHealth->SetFillColorAndOpacity(NewColor);
	}
}

FLinearColor USTBossBarWidget::ComputeHPColor(float Ratio01) const
{
	const FLinearColor Green   = FLinearColor(0.13f, 0.85f, 0.25f, 1.f);
	const FLinearColor Yellow  = FLinearColor(0.99f, 0.83f, 0.11f, 1.f);
	const FLinearColor Orange  = FLinearColor(0.95f, 0.49f, 0.10f, 1.f);
	const FLinearColor Red     = FLinearColor(0.90f, 0.12f, 0.12f, 1.f);
	const FLinearColor DeepRed = FLinearColor(0.75f, 0.05f, 0.05f, 1.f);

	
	if (Ratio01 >= 0.75f)
	{
		const float T = (Ratio01 - 0.75f) / 0.25f; // 0~1
		return FLinearColor::LerpUsingHSV(Yellow, Green, T);
	}
	
	else if (Ratio01 >= 0.50f)
	{
		const float T = (Ratio01 - 0.50f) / 0.25f;
		return FLinearColor::LerpUsingHSV(Orange, Yellow, T);
	}
	
	else if (Ratio01 >= CriticalThreshold)
	{
		const float T = (Ratio01 - CriticalThreshold) / (0.50f - CriticalThreshold);
		return FLinearColor::LerpUsingHSV(Red, Orange, T);
	}
	
	else
	{
		return DeepRed;
	}
}