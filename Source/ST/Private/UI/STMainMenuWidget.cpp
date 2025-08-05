#include "UI/STMainMenuWidget.h"

#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

USTMainMenuWidget::USTMainMenuWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

void USTMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_NewGame)
	{
		Btn_NewGame->OnClicked.AddDynamic(this, &USTMainMenuWidget::HandleNewGameClicked);
	}

	if (Btn_QuitGame)
	{
		Btn_QuitGame->OnClicked.AddDynamic(this, &USTMainMenuWidget::HandleQuitClicked);
	}
}

void USTMainMenuWidget::HandleNewGameClicked()
{
	if (OnNewGameRequested.IsBound())
	{
		OnNewGameRequested.Broadcast();  // 게임모드에 연결된 경우
	}
	else
	{
		TempHandleNewGame();
	}
}

void USTMainMenuWidget::HandleQuitClicked()
{
	if (OnQuitRequested.IsBound())
	{
		OnQuitRequested.Broadcast();  // 게임모드에 연결된 경우
	}
	else
	{
		TempHandleQuitGame();
	}
}

//임시
void USTMainMenuWidget::TempHandleNewGame()
{

}

void USTMainMenuWidget::TempHandleQuitGame()
{
	
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}