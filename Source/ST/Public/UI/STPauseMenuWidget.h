#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STPauseMenuWidget.generated.h"

class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuitGameRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReturnToMainRequested);

UCLASS()
class ST_API USTPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintAssignable, Category = "PauseMenu")
	FOnReturnToMainRequested OnReturnToMainRequested;

	UPROPERTY(BlueprintAssignable, Category = "PauseMenu")
	FOnQuitGameRequested OnQuitGameRequested;

protected:
	UPROPERTY(meta = (BindWidget)) UButton* Btn_ReturnToMain;
	UPROPERTY(meta = (BindWidget)) UButton* Btn_ResumeGame;
	UPROPERTY(meta = (BindWidget)) UButton* Btn_QuitGame;

	UFUNCTION() void OnReturnToMainClicked();
	UFUNCTION() void OnResumeClicked();
	UFUNCTION() void OnQuitClicked();

	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
};