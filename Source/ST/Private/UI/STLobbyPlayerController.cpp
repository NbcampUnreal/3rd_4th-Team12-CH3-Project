#include "UI/STLobbyPlayerController.h"
#include "UI/STLobbyWidget.h"
#include "Blueprint/UserWidget.h"
#include "System/STGameInstance.h"
#include "UI/STMainMenuPlayerController.h"
#include "System/STLog.h"

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
			// JM 캐릭터 선택시 델리게이트 바인딩
			LobbyWidgetInstance->OnCharacterSelected.AddDynamic(this, &ASTLobbyPlayerController::HandleCharacterSelected);
			LobbyWidgetInstance->OnBackToTitleRequested.AddDynamic(this, &ASTLobbyPlayerController::OnBackToTitleRequested);
			
			LobbyWidgetInstance->AddToViewport();

			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(LobbyWidgetInstance->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InputMode);

			bShowMouseCursor = true;
		}
	}
}

void ASTLobbyPlayerController::HandleCharacterSelected(const ECharacterType& CharacterType)
{
	UE_LOG(LogSystem, Warning, TEXT("ASTLobbyPlayerController::HandleCharacterSelected(%s) Start"), *StaticEnum<ECharacterType>()->GetValueAsString(CharacterType));
	if (USTGameInstance* STGameInstance = GetGameInstance<USTGameInstance>())
	{
		STGameInstance->SelectedCharacter = CharacterType;
		STGameInstance->StartStage1();
	}
	UE_LOG(LogSystem, Warning, TEXT("ASTLobbyPlayerController::HandleCharacterSelected(%s) End"), *StaticEnum<ECharacterType>()->GetNameStringByValue((int64)CharacterType));
}

void ASTLobbyPlayerController::OnBackToTitleRequested()
{
	UE_LOG(LogSystem, Warning, TEXT("ASTLobbyPlayerController::OnBackToTitleRequested() Start"));
	if (USTGameInstance* STGameInstance = GetGameInstance<USTGameInstance>())
	{
		STGameInstance->LoadMainMenuLevel();
	}
	UE_LOG(LogSystem, Warning, TEXT("ASTLobbyPlayerController::OnBackToTitleRequested() End"));
}
