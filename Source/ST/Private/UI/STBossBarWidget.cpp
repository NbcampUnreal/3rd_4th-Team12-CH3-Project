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
	}
}