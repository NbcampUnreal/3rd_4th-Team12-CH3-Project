#include "UI/STMainMenuPlayerController.h"
#include "UI/STMainMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "System/STGameInstance.h"
#include "System/STLog.h"

/************ public *************/
ASTMainMenuPlayerController::ASTMainMenuPlayerController()
{
	bShowMouseCursor = true;
}

// JM: 버튼 Onclick 함수
void ASTMainMenuPlayerController::HandleNewGameRequested()
{
	UE_LOG(LogSystem, Log, TEXT("ASTMainMenuPlayerController::HandleNewGameRequested() Start"));
	
	LoadLobbyLevel_BP();	// NOTE: 로딩화면 때문에 BP에서만 구현해야 함
	
	UE_LOG(LogSystem, Log, TEXT("ASTMainMenuPlayerController::HandleNewGameRequested() End"));
}

void ASTMainMenuPlayerController::HandleQuitRequested() 
{
	UE_LOG(LogSystem, Log, TEXT("ASTMainMenuPlayerController::HandleQuitRequested() Start"));
	if (USTGameInstance* STGameInstance = GetGameInstance<USTGameInstance>())
	{
		STGameInstance->QuitGame();
	}
	UE_LOG(LogSystem, Log, TEXT("ASTMainMenuPlayerController::HandleQuitRequested() End"));
}

/************ protected *************/
void ASTMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (MainMenuWidgetClass)
	{
		MenuInstance = CreateWidget<USTMainMenuWidget>(this, MainMenuWidgetClass);
		if (MenuInstance)
		{
			// JM 델리게이트 바인딩 추가
			/*MenuInstance->OnNewGameRequested.AddDynamic(this, &ASTMainMenuPlayerController::HandleNewGameRequested);
			MenuInstance->OnQuitRequested.AddDynamic(this, &ASTMainMenuPlayerController::HandleQuitRequested);*/
			
			MenuInstance->AddToViewport();

			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(MenuInstance->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InputMode);
		}
	}
}