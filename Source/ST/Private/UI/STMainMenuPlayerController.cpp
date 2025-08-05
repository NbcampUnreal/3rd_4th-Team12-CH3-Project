#include "UI/STMainMenuPlayerController.h"
#include "UI/STMainMenuWidget.h"
#include "Blueprint/UserWidget.h"

ASTMainMenuPlayerController::ASTMainMenuPlayerController()
{
	bShowMouseCursor = true;
}

void ASTMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (MainMenuWidgetClass)
	{
		MenuInstance = CreateWidget<USTMainMenuWidget>(this, MainMenuWidgetClass);
		if (MenuInstance)
		{
			MenuInstance->AddToViewport();

			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(MenuInstance->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InputMode);
		}
	}
}