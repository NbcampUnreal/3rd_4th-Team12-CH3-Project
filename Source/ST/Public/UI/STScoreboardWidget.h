#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STScoreboardWidget.generated.h"

class UTextBlock;

UCLASS()
class ST_API USTScoreboardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 점수/킬 수 업데이트
	UFUNCTION(BlueprintCallable)
	void UpdateScoreAndKill(int32 Score, int32 KillCount);

	// 미션 목표 설명
	UFUNCTION(BlueprintCallable)
	void UpdateMissionGoal(const FString& GoalText);

	// 미션 진행도
	UFUNCTION(BlueprintCallable)
	void UpdateMissionProgress(const FString& ProgressText);

protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Score;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_MissionGoal;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_MissionProgress;
};