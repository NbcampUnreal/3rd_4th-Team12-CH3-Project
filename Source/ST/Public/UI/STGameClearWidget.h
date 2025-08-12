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
	void SetResultInfo(int32 Score, int32 HighScore);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRetryRequested);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReturnToMainRequested);

	UPROPERTY(BlueprintAssignable, Category="Event")
	FOnRetryRequested OnRetryRequested;

	UPROPERTY(BlueprintAssignable, Category="Event")
	FOnReturnToMainRequested OnReturnToMainRequested;
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget)) UTextBlock* Txt_Score;
	UPROPERTY(meta = (BindWidget)) UTextBlock* Txt_HighScore;

	UPROPERTY(meta = (BindWidget)) UButton* Btn_Retry;
	UPROPERTY(meta = (BindWidget)) UButton* Btn_ReturnToMain;
	
private:
	UFUNCTION() void HandleRetryClicked();
	UFUNCTION() void HandleReturnToMainClicked();
};
