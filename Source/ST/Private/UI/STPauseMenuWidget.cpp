#include "UI/STPauseMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"

void USTPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_ResumeGame)
		Btn_ResumeGame->OnClicked.AddDynamic(this, &USTPauseMenuWidget::OnResumeClicked);

	if (Btn_QuitGame)
		Btn_QuitGame->OnClicked.AddDynamic(this, &USTPauseMenuWidget::OnQuitClicked);
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