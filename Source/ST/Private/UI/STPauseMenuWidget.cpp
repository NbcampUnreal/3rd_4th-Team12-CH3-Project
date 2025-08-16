#include "UI/STPauseMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"

void USTPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);

	if (Btn_ResumeGame)
		Btn_ResumeGame->OnClicked.AddDynamic(this, &USTPauseMenuWidget::OnResumeClicked);

	if (Btn_QuitGame)
		Btn_QuitGame->OnClicked.AddDynamic(this, &USTPauseMenuWidget::OnQuitClicked);
	
	if (Btn_ReturnToMain)
		Btn_ReturnToMain->OnClicked.AddDynamic(this, &USTPauseMenuWidget::OnReturnToMainClicked);
}

void USTPauseMenuWidget::OnResumeClicked()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetPause(false);
		SetVisibility(ESlateVisibility::Hidden);
		PC->SetInputMode(FInputModeGameOnly());
		PC->bShowMouseCursor = false;
	}
}

void USTPauseMenuWidget::OnQuitClicked()
{
	OnQuitGameRequested.Broadcast();
}

void USTPauseMenuWidget::OnReturnToMainClicked()
{
	OnReturnToMainRequested.Broadcast();
}

FReply USTPauseMenuWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		OnResumeClicked();
		return FReply::Handled();
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}