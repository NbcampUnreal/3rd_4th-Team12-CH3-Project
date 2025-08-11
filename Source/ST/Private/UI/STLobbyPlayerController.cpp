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
			/*LobbyWidgetInstance->OnCharacterSelected.AddDynamic(this, &ASTLobbyPlayerController::HandleCharacterSelected);
			LobbyWidgetInstance->OnBackToTitleRequested.AddDynamic(this, &ASTLobbyPlayerController::OnBackToTitleRequested);*/
			
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
	UE_LOG(LogSystem, Log, TEXT("ASTLobbyPlayerController::HandleCharacterSelected(%s) Start"), *StaticEnum<ECharacterType>()->GetValueAsString(CharacterType));
	
	// TODO: 캐릭터 선택정보 저장하기
	if (USTGameInstance* STGameInstance = GetGameInstance<USTGameInstance>())
	{
		STGameInstance->SelectedCharacter = CharacterType;
		STGameInstance->LastStage = EStageType::Stage1;
		// STGameInstance->StartStage1();
	}
	
	StartStage1_BP();
	
	UE_LOG(LogSystem, Log, TEXT("ASTLobbyPlayerController::HandleCharacterSelected(%s) End"), *StaticEnum<ECharacterType>()->GetNameStringByValue((int64)CharacterType));
}

void ASTLobbyPlayerController::OnBackToTitleRequested()
{
	UE_LOG(LogSystem, Log, TEXT("ASTLobbyPlayerController::OnBackToTitleRequested() Start"));
	BackToMainMenu_BP();
	/*if (USTGameInstance* STGameInstance = GetGameInstance<USTGameInstance>())
	{
		STGameInstance->LoadMainMenuLevel();
	}*/
	UE_LOG(LogSystem, Log, TEXT("ASTLobbyPlayerController::OnBackToTitleRequested() End"));
}
