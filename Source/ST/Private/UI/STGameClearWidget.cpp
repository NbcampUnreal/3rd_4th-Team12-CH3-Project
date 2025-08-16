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

void USTGameClearWidget::SetResultInfo(int32 Score, int32 HighScore)
{
	if (Txt_Score)
		Txt_Score->SetText(FText::FromString(FString::Printf(TEXT("점수: %d"), Score)));

	if (Txt_HighScore)
		Txt_HighScore->SetText(FText::FromString(FString::Printf(TEXT("최고 기록: %d"), HighScore)));
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