#include "UI/STGameOverWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void USTGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Retry)
	{
		Btn_Retry->OnClicked.AddDynamic(this, &USTGameOverWidget::HandleRetryClicked);
	}
	if (Btn_ReturnToMain)
	{
		Btn_ReturnToMain->OnClicked.AddDynamic(this, &USTGameOverWidget::HandleReturnToMainClicked);
	}

	// ✅ 중앙 정렬 적용
	if (Txt_Score) Txt_Score->SetJustification(ETextJustify::Center);
	if (Txt_KillCount) Txt_KillCount->SetJustification(ETextJustify::Center);
	if (Txt_DamageDealt) Txt_DamageDealt->SetJustification(ETextJustify::Center);
	if (Txt_DamageTaken) Txt_DamageTaken->SetJustification(ETextJustify::Center);
}

void USTGameOverWidget::SetResultInfo(int32 Score, int32 KillCount, int32 DamageDealt, int32 DamageTaken)
{
	if (Txt_Score)
		Txt_Score->SetText(FText::Format(NSLOCTEXT("GameOver", "ScoreLabel", "점수 : {0}"), Score));

	if (Txt_KillCount)
		Txt_KillCount->SetText(FText::Format(NSLOCTEXT("GameOver", "KillCountLabel", "처치 적 수 : {0}"), KillCount));

	if (Txt_DamageDealt)
		Txt_DamageDealt->SetText(FText::Format(NSLOCTEXT("GameOver", "DamageDealtLabel", "가한 피해량 : {0}"), DamageDealt));

	if (Txt_DamageTaken)
		Txt_DamageTaken->SetText(FText::Format(NSLOCTEXT("GameOver", "DamageTakenLabel", "받은 피해량 : {0}"), DamageTaken));
}


void USTGameOverWidget::HandleRetryClicked()
{
	OnRetryRequested.Broadcast();
}

void USTGameOverWidget::HandleReturnToMainClicked()
{
	OnReturnToMainRequested.Broadcast();
}