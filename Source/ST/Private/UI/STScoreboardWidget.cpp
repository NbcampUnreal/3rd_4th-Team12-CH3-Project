#include "UI/STScoreboardWidget.h"
#include "Components/TextBlock.h"

void USTScoreboardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 기본 텍스트 초기화
	UpdateScoreAndKill(0, 0);
	UpdateMissionGoal(TEXT("미션 목표 없음"));
	UpdateMissionProgress(TEXT("진행 정보 없음"));
}

void USTScoreboardWidget::UpdateScoreAndKill(int32 Score, int32 KillCount)
{
	if (Txt_Score)
	{
		FString Text = FString::Printf(TEXT("점수: %d | 킬: %d"), Score, KillCount);
		Txt_Score->SetText(FText::FromString(Text));
	}
}

void USTScoreboardWidget::UpdateMissionGoal(const FString& GoalText)
{
	if (Txt_MissionGoal)
	{
		Txt_MissionGoal->SetText(FText::FromString(GoalText));
	}
}

void USTScoreboardWidget::UpdateMissionProgress(const FString& ProgressText)
{
	if (Txt_MissionProgress)
	{
		Txt_MissionProgress->SetText(FText::FromString(ProgressText));
	}
}