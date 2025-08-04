#include "System/STGameInstance.h"
#include "Kismet/GameplayStatics.h"

USTGameInstance::USTGameInstance()
{
	ResetGame();	// 단순 변수 초기화는 생성자에서 호출해도 괜찮음 (언리얼 객체 등에 접근하려면 로드가 완료된 후, 별도의 초기화 타이밍에 해야함)
}

void USTGameInstance::SaveGameData()
{
	// TODO: 게임 진행상황 Save 기능 구현
}

void USTGameInstance::LoadGameData()
{
	// TODO: 게임 진행상황 Load 기능 구현
}

void USTGameInstance::ResetGame()
{
	SelectedCharacter = ECharacterType::None;
	LastStage = EStageType::None;
	bIsContinue = false;
	/*MasterVolume = 1.0f;*/
}

void USTGameInstance::GoToLevel(EStageType StageType)
{
	// TODO: 레벨 이동 전 세이브, 상태저장, 로딩화면, 사운드 등 제어 가능

	FName TargetLevel = FName("MainMenu");	// Default 값

	switch (StageType)
	{
		case EStageType::MainMenu:	TargetLevel = FName("MainMenu"); break;
		case EStageType::Lobby:		TargetLevel = FName("Lobby"); break;
		case EStageType::Stage1:	TargetLevel = FName("Stage1"); break;
		case EStageType::Stage2:	TargetLevel = FName("Stage2"); break;
		case EStageType::Stage3:	TargetLevel = FName("Stage3"); break;
		case EStageType::Ending:	TargetLevel = FName("Ending"); break;
		default:					TargetLevel = FName("MainMenu"); break;
	}
	
	UGameplayStatics::OpenLevel(GetWorld(), TargetLevel);
}

void USTGameInstance::StartNewGame()
{
	ResetGame();
	GoToLevel(EStageType::Lobby);
}

void USTGameInstance::StartContinuousGame()
{
	LoadGameData();
	bIsContinue = true;
	GoToLevel(LastStage);
}

void USTGameInstance::QuitGame()
{
	UWorld* World = GetWorld();
	APlayerController* PlayerController = nullptr;
	if (World)
		PlayerController = World->GetFirstPlayerController();

	UKismetSystemLibrary::QuitGame(World, PlayerController, EQuitPreference::Quit, true);
}
