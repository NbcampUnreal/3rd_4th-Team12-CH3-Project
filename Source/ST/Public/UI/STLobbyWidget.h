#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "System/STGameTypes.h"
#include "STLobbyWidget.generated.h"

class UButton;

UCLASS()
class ST_API USTLobbyWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    USTLobbyWidget(const FObjectInitializer& ObjectInitializer);

    /** GameMode에 넘겨줄 캐릭터 선택 델리게이트 */
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterSelected, const ECharacterType&, CharacterType);
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBackToTitleRequested);

    /** GameMode에서 바인딩할 델리게이트 */
    UPROPERTY(BlueprintAssignable, Category = "Event")
    FOnCharacterSelected OnCharacterSelected;

    UPROPERTY(BlueprintAssignable, Category = "Event")
    FOnBackToTitleRequested OnBackToTitleRequested;

protected:
    virtual void NativeConstruct() override;

    /** UI에 바인딩될 버튼 */
    UPROPERTY(meta = (BindWidget)) UButton* Btn_SelectMale;

    UPROPERTY(meta = (BindWidget)) UButton* Btn_SelectFemale;

    UPROPERTY(meta = (BindWidget)) UButton* Btn_Back;

    /** 선택 결과 임시 표시용 (GameMode에서 처리될 값들) */
    UPROPERTY(meta = (BindWidgetOptional)) class UTextBlock* Txt_ResultText;

private:
    UFUNCTION() void HandleSelectMale();
    UFUNCTION() void HandleSelectFemale();
    UFUNCTION() void HandleBack();

    void ShowTempMessage(const FString& Message);
};
