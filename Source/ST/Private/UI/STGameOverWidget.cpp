#include "UI/STGameOverWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void USTGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void USTGameOverWidget::SetResultInfo(int32 Score, int32 KillCount, int32 DamageDealt, int32 DamageTaken)
{
	if (Txt_Score)
		Txt_Score->SetText(FText::AsNumber(Score));

	if (Txt_KillCount)
		Txt_KillCount->SetText(FText::AsNumber(KillCount));

	if (Txt_DamageDealt)
		Txt_DamageDealt->SetText(FText::AsNumber(DamageDealt));

	if (Txt_DamageTaken)
		Txt_DamageTaken->SetText(FText::AsNumber(DamageTaken));
}