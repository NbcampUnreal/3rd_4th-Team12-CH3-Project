#include "UI/STStagePlayerController.h"
#include "UI/STStageWidget.h"
#include "UI/STPauseMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "System/STGameInstance.h"
#include "Kismet/GameplayStatics.h"


ASTStagePlayerController::ASTStagePlayerController()
{
	bShowMouseCursor = false;
}

void ASTStagePlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (StageWidgetClass)
	{
		StageWidget = CreateWidget<USTStageWidget>(this, StageWidgetClass);
		if (StageWidget)
		{
			StageWidget->AddToViewport();
		}
	}

	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
}

void ASTStagePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 테스트용 키 (P)
	InputComponent->BindKey(EKeys::P, IE_Pressed, this, &ASTStagePlayerController::TogglePauseMenu);
}

void ASTStagePlayerController::TogglePauseMenu()
{
	if (IsPaused())
	{
		// 게임 재개
		SetPause(false);

		if (PauseMenuWidget)
		{
			PauseMenuWidget->SetVisibility(ESlateVisibility::Hidden);
		}

		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;
	}
	else
	{
		// 게임 일시정지
		SetPause(true);

		if (!PauseMenuWidget && PauseMenuWidgetClass)
		{
			PauseMenuWidget = CreateWidget<USTPauseMenuWidget>(this, PauseMenuWidgetClass);
			if (PauseMenuWidget)
			{
				PauseMenuWidget->AddToViewport(100);
				PauseMenuWidget->OnQuitGameRequested.AddDynamic(this, &ASTStagePlayerController::HandleQuitGame);
			}
		}

		if (PauseMenuWidget)
		{
			PauseMenuWidget->SetVisibility(ESlateVisibility::Visible);
		}

		SetInputMode(FInputModeUIOnly());
		bShowMouseCursor = true;
	}
}

void ASTStagePlayerController::UpdateHealth(float CurrentHP, float MaxHP)
{
	if (StageWidget)
		StageWidget->UpdateHealth(CurrentHP, MaxHP);
}

void ASTStagePlayerController::UpdateWeapon(const FString& WeaponName)
{
	if (StageWidget)
		StageWidget->UpdateWeapon(WeaponName);
}

void ASTStagePlayerController::UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (StageWidget)
		StageWidget->UpdateAmmo(CurrentAmmo, MaxAmmo);
}

void ASTStagePlayerController::UpdateTimer(int32 RemainingSeconds)
{
	if (StageWidget)
		StageWidget->UpdateTimer(RemainingSeconds);
}

void ASTStagePlayerController::UpdateEnemyStatus(int32 Killed, int32 Total)
{
	if (StageWidget)
		StageWidget->UpdateEnemyStatus(Killed, Total);
}

void ASTStagePlayerController::AddDamageKillLog(const FString& LogText)
{
	if (StageWidget)
		StageWidget->AddDamageKillLog(LogText);
}

void ASTStagePlayerController::ShowHitMarker()
{
	if (StageWidget)
		StageWidget->ShowHitMarker();
}

void ASTStagePlayerController::ShowKillConfirmed()
{
	if (StageWidget)
		StageWidget->ShowKillConfirmedImage();
}

void ASTStagePlayerController::ShowDamageTextAt(FVector WorldLocation, int32 Damage)
{
	if (StageWidget)
		StageWidget->ShowDamageTextAt(WorldLocation, Damage);
}

void ASTStagePlayerController::HandleQuitGame()
{
	//임시코드(여기에서 종료)
	UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
}