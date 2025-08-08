// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/STGameClearWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"


void USTGameClearWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 버튼 바인딩 필요 시 추가
}

void USTGameClearWidget::SetResultInfo(int32 Score, int32 HighScore)
{
	if (Txt_Score)
		Txt_Score->SetText(FText::FromString(FString::Printf(TEXT("점수: %d"), Score)));

	if (Txt_HighScore)
		Txt_HighScore->SetText(FText::FromString(FString::Printf(TEXT("최고 기록: %d"), HighScore)));
}