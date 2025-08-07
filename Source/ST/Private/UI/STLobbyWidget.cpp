#include "UI/STLobbyWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

USTLobbyWidget::USTLobbyWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{}

void USTLobbyWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Btn_SelectMale)
        Btn_SelectMale->OnClicked.AddDynamic(this, &USTLobbyWidget::HandleSelectMale);

    if (Btn_SelectFemale)
        Btn_SelectFemale->OnClicked.AddDynamic(this, &USTLobbyWidget::HandleSelectFemale);

    if (Btn_Back)
        Btn_Back->OnClicked.AddDynamic(this, &USTLobbyWidget::HandleBack);
}

void USTLobbyWidget::HandleSelectMale()
{
    // JM 수정 (FString& -> ECharacterType& 으로 변경)
    OnCharacterSelected.Broadcast((ECharacterType::JaxMercer));
    // OnCharacterSelected.Broadcast(TEXT("Male"));

}

void USTLobbyWidget::HandleSelectFemale()
{
    // JM 수정 (FString& -> ECharacterType& 으로 변경)
    OnCharacterSelected.Broadcast((ECharacterType::AvaRaines));
    // OnCharacterSelected.Broadcast(TEXT("Female"));

}

void USTLobbyWidget::HandleBack()
{
    OnBackToTitleRequested.Broadcast();

}

void USTLobbyWidget::ShowTempMessage(const FString& Message)
{
    if (Txt_ResultText)
    {
        Txt_ResultText->SetText(FText::FromString(Message));
    }
}