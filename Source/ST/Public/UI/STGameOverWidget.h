#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STGameOverWidget.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class ST_API USTGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 게임 결과를 전달받아 텍스트를 업데이트하는 함수
	UFUNCTION(BlueprintCallable)
	void SetResultInfo(int32 Score, int32 KillCount, int32 DamageDealt, int32 DamageTaken, const FText& ReasonText);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRetryRequested);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReturnToMainRequested);

	UPROPERTY(BlueprintAssignable, Category="Event")
	FOnRetryRequested OnRetryRequested;

	UPROPERTY(BlueprintAssignable, Category="Event")
	FOnReturnToMainRequested OnReturnToMainRequested;
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Score;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_KillCount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_DamageDealt;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_DamageTaken;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_GameOverReason;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_Retry;

	UPROPERTY(meta = (BindWidget))
	UButton* Btn_ReturnToMain;
	
private:
	UFUNCTION() void HandleRetryClicked();
	UFUNCTION() void HandleReturnToMainClicked();
};