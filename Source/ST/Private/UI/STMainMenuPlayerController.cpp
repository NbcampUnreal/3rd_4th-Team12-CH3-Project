#include "UI/STMainMenuPlayerController.h"
#include "UI/STMainMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "System/STGameInstance.h"
#include "System/STLog.h"

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
			// JM 델리게이트 바인딩 추가
			MenuInstance->OnNewGameRequested.AddDynamic(this, &ASTMainMenuPlayerController::HandleNewGameRequested);
			MenuInstance->OnQuitRequested.AddDynamic(this, &ASTMainMenuPlayerController::HandleQuitRequested);
			
			MenuInstance->AddToViewport();

			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(MenuInstance->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InputMode);
		}
	}
}

// JM 시작버튼 델리게이트 추가
void ASTMainMenuPlayerController::HandleNewGameRequested()
{
	UE_LOG(LogSystem, Warning, TEXT("ASTMainMenuPlayerController::HandleNewGameRequested() Start"));

	
	
	LoadLobbyLevel_BP();
	
	/*
	if (USTGameInstance* STGameInstance = GetGameInstance<USTGameInstance>())
	{
		
		// STGameInstance->StartNewGame();
	}*/
	UE_LOG(LogSystem, Warning, TEXT("ASTMainMenuPlayerController::HandleNewGameRequested() End"));
}

void ASTMainMenuPlayerController::HandleQuitRequested()
{
	UE_LOG(LogSystem, Warning, TEXT("ASTMainMenuPlayerController::HandleQuitRequested() Start"));
	if (USTGameInstance* STGameInstance = GetGameInstance<USTGameInstance>())
	{
		STGameInstance->QuitGame();
	}
	UE_LOG(LogSystem, Warning, TEXT("ASTMainMenuPlayerController::HandleQuitRequested() End"));
}
