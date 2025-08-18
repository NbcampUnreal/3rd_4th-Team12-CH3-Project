// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/STGameClearWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"


void USTGameClearWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Btn_Retry)
	{
		Btn_Retry->OnClicked.AddDynamic(this, &USTGameClearWidget::HandleRetryClicked);
	}
	if (Btn_ReturnToMain)
	{
		Btn_ReturnToMain->OnClicked.AddDynamic(this, &USTGameClearWidget::HandleReturnToMainClicked);
	}
	if (Btn_Ending)
	{
		Btn_Ending->OnClicked.AddDynamic(this, &USTGameClearWidget::HandlePlayEndingClicked);
	}
}

void USTGameClearWidget::SetResultInfo(
	int32 Score,
	int32 HighScore,
	int32 DamageDealt,
	int32 DamageTaken,
	int32 BulletsUsed,
	int32 EnemiesKilled
)
{
	// 점수/최고기록
	if (Txt_Score)
	{
		Txt_Score->SetText(FText::Format(
			NSLOCTEXT("GameClear", "ScoreFmt", "점수: {0}"),
			FText::AsNumber(Score)
		));
	}
	if (Txt_HighScore)
	{
		Txt_HighScore->SetText(FText::Format(
			NSLOCTEXT("GameClear", "HighScoreFmt", "최고 기록: {0}"),
			FText::AsNumber(HighScore)
		));
	}

	// 전투 통계
	if (Txt_DamageDealt)
	{
		Txt_DamageDealt->SetText(FText::Format(
			NSLOCTEXT("GameClear", "DamageDealtFmt", "가한 피해량: {0}"),
			FText::AsNumber(DamageDealt)
		));
	}
	if (Txt_DamageTaken)
	{
		Txt_DamageTaken->SetText(FText::Format(
			NSLOCTEXT("GameClear", "DamageTakenFmt", "받은 피해량: {0}"),
			FText::AsNumber(DamageTaken)
		));
	}
	if (Txt_BulletsUsed)
	{
		Txt_BulletsUsed->SetText(FText::Format(
			NSLOCTEXT("GameClear", "BulletsUsedFmt", "사용한 총알 수: {0}"),
			FText::AsNumber(BulletsUsed)
		));
	}
	if (Txt_EnemiesKilled)
	{
		Txt_EnemiesKilled->SetText(FText::Format(
			NSLOCTEXT("GameClear", "EnemiesKilledFmt", "처치한 적 수: {0}"),
			FText::AsNumber(EnemiesKilled)
		));
	}
}

void USTGameClearWidget::HandleRetryClicked()
{
	OnRetryRequested.Broadcast();
}

void USTGameClearWidget::HandleReturnToMainClicked()
{
	OnReturnToMainRequested.Broadcast();
}

void USTGameClearWidget::HandlePlayEndingClicked()
{
	OnPlayEndingRequested.Broadcast();
}