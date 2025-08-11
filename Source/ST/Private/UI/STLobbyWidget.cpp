#include "UI/STLobbyWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "System/STLog.h"
#include "UI/STLobbyPlayerController.h"

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
    UE_LOG(LogSystem, Log, TEXT("USTLobbyWidget::HandleSelectMale() Start"));
    
    // TODO: GetOwningPlayerController()로 변경하기
    if (ASTLobbyPlayerController* STLobbyPC = Cast<ASTLobbyPlayerController>(GetWorld()->GetFirstPlayerController()))
    {
        STLobbyPC->HandleCharacterSelected(ECharacterType::JaxMercer);
    }

    
    // JM 수정 (FString& -> ECharacterType& 으로 변경)
    // OnCharacterSelected.Broadcast((ECharacterType::JaxMercer));

    UE_LOG(LogSystem, Log, TEXT("USTLobbyWidget::HandleSelectMale() Start"));
}

void USTLobbyWidget::HandleSelectFemale()
{
    UE_LOG(LogSystem, Log, TEXT("USTLobbyWidget::HandleSelectFemale() Start"));
    
    
    // TODO: GetOwningPlayerController()로 변경하기
    if (ASTLobbyPlayerController* STLobbyPC = Cast<ASTLobbyPlayerController>(GetWorld()->GetFirstPlayerController()))
    {
        STLobbyPC->HandleCharacterSelected(ECharacterType::AvaRaines);
    }
    
    // JM 수정 (FString& -> ECharacterType& 으로 변경)
    // OnCharacterSelected.Broadcast((ECharacterType::AvaRaines));
    

    
    UE_LOG(LogSystem, Log, TEXT("USTLobbyWidget::HandleSelectFemale() End"));
}

void USTLobbyWidget::HandleBack()
{
    UE_LOG(LogSystem, Log, TEXT("USTLobbyWidget::HandleBack() Start"));
    
    // TODO: GetOwningPlayerController()로 변경하기
    if (ASTLobbyPlayerController* STLobbyPC = Cast<ASTLobbyPlayerController>(GetWorld()->GetFirstPlayerController()))
    {
        STLobbyPC->OnBackToTitleRequested();
    }
    
    // OnBackToTitleRequested.Broadcast();
    
    UE_LOG(LogSystem, Log, TEXT("USTLobbyWidget::HandleBack() End"));
}

void USTLobbyWidget::ShowTempMessage(const FString& Message)
{
    if (Txt_ResultText)
    {
        Txt_ResultText->SetText(FText::FromString(Message));
    }
}