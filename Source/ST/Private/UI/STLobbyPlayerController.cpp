#include "UI/STLobbyPlayerController.h"
#include "UI/STLobbyWidget.h"
#include "Blueprint/UserWidget.h"

ASTLobbyPlayerController::ASTLobbyPlayerController()
{
	bShowMouseCursor = true;
}

void ASTLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (LobbyWidgetClass)
	{
		LobbyWidgetInstance = CreateWidget<USTLobbyWidget>(this, LobbyWidgetClass);
		if (LobbyWidgetInstance)
		{
			LobbyWidgetInstance->AddToViewport();

			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(LobbyWidgetInstance->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InputMode);

			bShowMouseCursor = true;
		}
	}
}