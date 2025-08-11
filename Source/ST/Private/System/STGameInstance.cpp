#include "System/STGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "System/STLog.h"

/************** public functions **************/
USTGameInstance::USTGameInstance()
{
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::Constructor() Start"));
	
	ResetGameData();
	
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::Constructor() End"));
}


void USTGameInstance::StartNewGame()
{
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::StartNewGame() Start"));
	
	ResetGameData();
	GoToLevel(EStageType::Lobby);	// 새 게임은 항상 로비에서 캐릭터를 선택하면서 시작함
	
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::StartNewGame() End"));
}

void USTGameInstance::GoToNextStage()
{
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::GoToNextStage() Start"));
	
	EStageType NextStage = GetNextStageType(LastStage);
	if (NextStage != EStageType::None)
	{
		GoToLevel(NextStage);
	}
	else
	{
		UE_LOG(LogSystem, Log, TEXT("USTGameInstance::GoToNextStage() All Stage Cleared."));
		// GoToLevel(EStageType::Ending); // TODO: 엔딩 레벨로 이동
	}
	
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::GoToNextStage() End"));
}

void USTGameInstance::GoToLevel(EStageType StageType)
{
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::GoToLevel(%s) Start"), *StaticEnum<EStageType>()->GetValueAsString(StageType));

	// TODO: 레벨 이동 전 보존할 정보 저장
	
	
	
	FName TargetLevelName = FName("MainMenu");	// Default 값

	switch (StageType)
	{
		case EStageType::MainMenu:	TargetLevelName = FName("MainMenu"); break;
		case EStageType::Lobby:		TargetLevelName = FName("LobbyLevel"); break;
		case EStageType::Stage1:	TargetLevelName = FName("Stage1"); break;
		case EStageType::Stage2:	TargetLevelName = FName("Stage2"); break;
		case EStageType::Stage3:	TargetLevelName = FName("Stage3"); break;
		case EStageType::Ending:	TargetLevelName = FName("Ending"); break;
		default:					break;
	}
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::GoToLevel(%s) OpenLevel(%s)"), *StaticEnum<EStageType>()->GetValueAsString(StageType), *TargetLevelName.ToString());
	UGameplayStatics::OpenLevel(GetWorld(), TargetLevelName);
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::GoToLevel(%s) End"), *StaticEnum<EStageType>()->GetValueAsString(StageType));
}

void USTGameInstance::GoToMainMenu()
{
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::GoToMainMenu() Start"));

	GoToLevel(EStageType::MainMenu);
	
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::GoToMainMenu() End"));
}


void USTGameInstance::QuitGame()
{
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::QuitGame() Start"));
	UWorld* World = GetWorld();
	APlayerController* PlayerController = nullptr;
	if (World)
		PlayerController = World->GetFirstPlayerController();

	UKismetSystemLibrary::QuitGame(World, PlayerController, EQuitPreference::Quit, true);
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::QuitGame() End"));
}


/************** private functions **************/
EStageType USTGameInstance::GetNextStageType(EStageType CurrentStage) const
{
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::GetNextStageType() Start"));

	switch (CurrentStage)
	{
		case EStageType::MainMenu :
		case EStageType::Lobby :
			UE_LOG(LogSystem, Log, TEXT("USTGameInstance::GetNextStageType() End return Stage1"));
			return EStageType::Stage1;
		case EStageType::Stage1 :
			UE_LOG(LogSystem, Log, TEXT("USTGameInstance::GetNextStageType() End return Stage2"));
			return EStageType::Stage2;
		case EStageType::Stage2:
			UE_LOG(LogSystem, Log, TEXT("USTGameInstance::GetNextStageType() End return Stage3"));
			return EStageType::Stage3;	
		case EStageType::Stage3 :
			UE_LOG(LogSystem, Log, TEXT("USTGameInstance::GetNextStageType() End return Ending"));
			return EStageType::Ending;	// 보스전 종료 후 엔딩 레벨로
		default:
			UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::GetNextStageType() End return NONE"));
			return EStageType::None;
	}
}

void USTGameInstance::ResetGameData()
{
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::ResetGameData() Start"));

	// TODO: 일일히 변수 하나씩 하지말고 struct로 확장필요
	LastStage = EStageType::None;
	SelectedCharacter = ECharacterType::None;
	
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::ResetGameData() End"));
}



/*
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
	/*MasterVolume = 1.0f;#1#
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::ResetGame() End"));
}

void USTGameInstance::GoToLevel(EStageType StageType)
{
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::GoToLevel(%s) Start"), *StaticEnum<EStageType>()->GetValueAsString(StageType));
	// TODO: 레벨 이동 전 세이브, 상태저장, 로딩화면, 사운드 등 제어 가능
	FName TargetLevel = FName("MainMenu");	// Default 값

	switch (StageType)
	{
		case EStageType::MainMenu:	TargetLevel = FName("MainMenu"); break;
		case EStageType::Lobby:		TargetLevel = FName("LobbyLevel"); break;
		case EStageType::Stage1:	TargetLevel = FName("Stage1"); break;
		case EStageType::Stage2:	TargetLevel = FName("Stage2"); break;
		case EStageType::Stage3:	TargetLevel = FName("Stage3"); break;
		case EStageType::Ending:	TargetLevel = FName("Ending"); break;
		default:					TargetLevel = FName("MainMenu"); break;
	}
	
	UGameplayStatics::OpenLevel(GetWorld(), TargetLevel);
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::GoToLevel(%s) End"), *StaticEnum<EStageType>()->GetValueAsString(StageType));
}

void USTGameInstance::LoadMainMenuLevel()
{
	// TODO: 불필요한 Rapper 인가..? 메인메뉴로 돌아갈 때 추가로 작업해야 할 게 있지 않을까?
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::LoadMainMenuLevel() Start"));
	GoToLevel(EStageType::MainMenu);
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::LoadMainMenuLevel() End"));
}

void USTGameInstance::StartNewGame()
{
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::StartNewGame() Start"));
	
	ResetGame();
	GoToLevel(EStageType::Lobby);
	
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::StartNewGame() End"));
}

void USTGameInstance::StartStage1()
{
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::StartStage1() Start"));
	// GameInstance 설정
	LastStage = EStageType::Stage1;
	GoToLevel(EStageType::Stage1);
	UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::StartStage1() End"));
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
}*/
