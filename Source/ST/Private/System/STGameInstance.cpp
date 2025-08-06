#include "System/STGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "System/STLog.h"

USTGameInstance::USTGameInstance()
{
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::Constructor() Start"));
	ResetGame();	// 단순 변수 초기화는 생성자에서 호출해도 괜찮음 (언리얼 객체 등에 접근하려면 로드가 완료된 후, 별도의 초기화 타이밍에 해야함)
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::Constructor() End"));
}

void USTGameInstance::SaveGameData()
{
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::SaveGameData() Start"));
	// TODO: 게임 진행상황 Save 기능 구현
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::SaveGameData() End"));
}

void USTGameInstance::LoadGameData()
{
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::LoadGameData() Start"));
	// TODO: 게임 진행상황 Load 기능 구현
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::LoadGameData() End"));
}

void USTGameInstance::ResetGame()
{
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::ResetGame() Start"));
	SelectedCharacter = ECharacterType::None;
	LastStage = EStageType::None;
	bIsContinue = false;
	/*MasterVolume = 1.0f;*/
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::ResetGame() End"));
}

void USTGameInstance::GoToLevel(EStageType StageType)
{
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::GoToLevel() Start"));
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
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::GoToLevel() End"));
}

void USTGameInstance::StartNewGame()
{
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::StartNewGame() Start"));
	
	ResetGame();
	GoToLevel(EStageType::Lobby);
	
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::StartNewGame() End"));
}

void USTGameInstance::StartContinuousGame()
{
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::StartContinuousGame() Start"));
	LoadGameData();
	bIsContinue = true;
	GoToLevel(LastStage);
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::StartContinuousGame() End"));
}

void USTGameInstance::QuitGame()
{
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::QuitGame() Start"));
	UWorld* World = GetWorld();
	APlayerController* PlayerController = nullptr;
	if (World)
		PlayerController = World->GetFirstPlayerController();

	UKismetSystemLibrary::QuitGame(World, PlayerController, EQuitPreference::Quit, true);
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::QuitGame() End"));
}
