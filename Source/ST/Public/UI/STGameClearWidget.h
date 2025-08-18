// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STGameClearWidget.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class ST_API USTGameClearWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetResultInfo(
		int32 Score,
		int32 HighScore,
		int32 DamageDealt,
		int32 DamageTaken,
		int32 BulletsUsed,
		int32 EnemiesKilled
	);
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRetryRequested);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReturnToMainRequested);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayEndingRequested);
	
	UPROPERTY(BlueprintAssignable, Category="Event") FOnRetryRequested OnRetryRequested;
	UPROPERTY(BlueprintAssignable, Category="Event") FOnReturnToMainRequested OnReturnToMainRequested;
	UPROPERTY(BlueprintAssignable, Category="Event") FOnPlayEndingRequested OnPlayEndingRequested;
	
protected:
	virtual void NativeConstruct() override;

	// 게임 결과
	UPROPERTY(meta = (BindWidget)) UTextBlock* Txt_DamageDealt;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Txt_DamageTaken;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Txt_BulletsUsed;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Txt_EnemiesKilled;

	// 점수
	UPROPERTY(meta = (BindWidget)) UTextBlock* Txt_Score;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Txt_HighScore;

	// 버튼
	UPROPERTY(meta = (BindWidget)) UButton* Btn_Retry;
	UPROPERTY(meta = (BindWidget)) UButton* Btn_ReturnToMain;
	UPROPERTY(meta = (BindWidget)) UButton* Btn_Ending;
	
private:
	UFUNCTION() void HandleRetryClicked();
	UFUNCTION() void HandleReturnToMainClicked();
	UFUNCTION() void HandlePlayEndingClicked();
};
