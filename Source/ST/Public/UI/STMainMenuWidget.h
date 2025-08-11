#pragma once


#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STMainMenuWidget.generated.h"
class UButton;

UCLASS()
class ST_API USTMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	USTMainMenuWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget)) UButton* Btn_NewGame;
	UPROPERTY(meta = (BindWidget)) UButton* Btn_QuitGame;

	// 테스트용 직접 처리 함수
	void TempHandleNewGame();
	void TempHandleQuitGame();

	// 게임모드에 넘길 델리게이트는 여전히 유지 (후에 연결할 수 있도록)
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNewGameRequested);	// JM : 델리게이트 없이 바로 MainmenuPC 함수 호출하는게?
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQuitRequested);		// JM : 델리게이트 없이 바로 MainmenuPC 함수 호출하는게?

	UPROPERTY(BlueprintAssignable)
	FOnNewGameRequested OnNewGameRequested;

	UPROPERTY(BlueprintAssignable)
	FOnQuitRequested OnQuitRequested;

private:
	UFUNCTION()
	void HandleNewGameClicked();

	UFUNCTION()
	void HandleQuitClicked();
};