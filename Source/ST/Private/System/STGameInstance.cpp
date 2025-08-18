#include "System/STGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Player/STPlayerBaseData.h"
#include "System/STGameMode.h"
#include "System/STLog.h"
#include "System/STSaveGame.h"

/************** public functions **************/
USTGameInstance::USTGameInstance()
{
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::Constructor() Start"));
	
	ResetGameData();
	
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::Constructor() End"));
}

void USTGameInstance::Init() //  생성자에서는 BP가 NULL로 되어서 문제 발생 
{
	Super::Init();
	LoadSavedData();
	ResetPlayerStateInfo();
}

void USTGameInstance::StartNewGame()	// TODO: 더 이상 사용 X
{
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::StartNewGame() Start"));
	
	ResetGameData();
	GoToLevel(EStageType::Lobby);	// 새 게임은 항상 로비에서 캐릭터를 선택하면서 시작함
	
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::StartNewGame() End"));
}

void USTGameInstance::GoToNextStage()		// TODO: 이젠 사용 X
{
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::GoToNextStage() Start"));

	GoToLevel(GetNextStageType(LastStage));
	
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::GoToNextStage() End"));
}

void USTGameInstance::GoToLevel(const EStageType StageType)
{
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::GoToLevel(%s) Start"), *StaticEnum<EStageType>()->GetValueAsString(StageType));
	
	FName TargetLevelName = FName("MainMenu");	// Default 값
	switch (StageType)
	{
		case EStageType::MainMenu:
			TargetLevelName = FName("MainMenu");
			LastStage = EStageType::MainMenu;
			break;
		case EStageType::Lobby:
			TargetLevelName = FName("LobbyLevel");
			LastStage = EStageType::Lobby;
			break;
		case EStageType::Stage1:	
			TargetLevelName = FName("Stage1");
			LastStage = EStageType::Stage1;
			break;
		case EStageType::Stage2:
			TargetLevelName = FName("Stage2");
			LastStage = EStageType::Stage2;
			break;
		case EStageType::Stage3:	
			TargetLevelName = FName("Stage3");
			LastStage = EStageType::Stage3;
			break;
		case EStageType::Ending:	
			TargetLevelName = FName("Ending");
			LastStage = EStageType::Ending;
			break;
		default:	// EStageType == None인 경우, MainMenu로 이동
			UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::GoToLevel(%s) StageType == None"), *StaticEnum<EStageType>()->GetValueAsString(StageType))
			break;
	}
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::GoToLevel(%s) OpenLevel(%s)"), *StaticEnum<EStageType>()->GetValueAsString(StageType), *TargetLevelName.ToString());
	UGameplayStatics::OpenLevel(GetWorld(), TargetLevelName);
	
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::GoToLevel(%s) End"), *StaticEnum<EStageType>()->GetValueAsString(StageType));
}

void USTGameInstance::GoToMainMenu()
{
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::GoToMainMenu() Start"));
  
	ResetDataForRetry();
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

void USTGameInstance::ResetDataForRetry()
{
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::ResetPlayerStateInfo() Start"));
	
	const int32 HighScore = PlayerStateInfo.HighScore;
	PlayerStateInfo = FPlayerStateInfo();
	ResetPlayerStateInfo();
	PlayerStateInfo.HighScore = HighScore;
	
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::ResetPlayerStateInfo() End"));
}

void USTGameInstance::GoToRetry()
{
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::GoToRetry() Start"));
	
	ResetDataForRetry();
	GoToLevel(EStageType::Stage1);
	
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::GoToRetry() End"));
}

void USTGameInstance::SetPlayerStateInfo(const FPlayerStateInfo& NewInfo)
{
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::SetPlayerStateInfo() Start"));
	
	PlayerStateInfo = NewInfo;
	
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::SetPlayerStateInfo() End"));
}

const FPlayerStateInfo& USTGameInstance::GetPlayerStateInfo() const
{
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::GetPlayerStateInfo() Start"));
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::GetPlayerStateInfo() End"));
	return PlayerStateInfo;
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
	
	SelectedCharacter = ECharacterType::None;
	
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::ResetGameData() End"));
}

void USTGameInstance::ResetPlayerStateInfo() // 준범 : player 정보 초기화 
{
	if (IsValid(PlayerDefaultDataAsset))
	{
		//hp
		PlayerStateInfo.MaxHP = PlayerDefaultDataAsset->BaseMaxHealth;
		PlayerStateInfo.CurrentHP = PlayerDefaultDataAsset->BaseMaxHealth;
		//movement
		PlayerStateInfo.MoveSpeed = PlayerDefaultDataAsset->BaseWalkSpeed;
		PlayerStateInfo.CrouchMultiplier = PlayerDefaultDataAsset->CrouchMultiplier;
		PlayerStateInfo.SprintMultiplier = PlayerDefaultDataAsset->SprintMultiplier;
		PlayerStateInfo.ZoomMultiplier = PlayerDefaultDataAsset->ZoomMultiplier;
		//weapon
		PlayerStateInfo.PlayerWeaponData.WeaponClass = PlayerDefaultDataAsset->BaseWeapon;
	}
}

void USTGameInstance::LoadSavedData()
{
	const FString SlotName = USTSaveGame::DefaultSlotName;
	const int32 SlotIndex = USTSaveGame::DefaultUserIndex;

	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		if (const USTSaveGame* LoadGame = Cast<USTSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex)))
		{
			PlayerStateInfo.HighScore = LoadGame->SaveData.HighScore;	// 지금은 HighScore만 저장하기
			UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::LoadSavedData() Success to Load SavedData(HighScore=%d)"), LoadGame->SaveData.HighScore);
		}
		else
		{
			UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::LoadSavedData() Load Success, but Cast Failed"));			
		}
	}
	else
	{
		// SaveData = FSaveData(); // 나중에 save data로 관리해야 한다면, 해당 구조체를 헤더에 추가.(지금은 PlayerStateInfo를 초기화해서 안해도 됨)
		UE_LOG(LogSystem, Warning, TEXT("USTGameInstance::LoadSavedData() Load Failed, No Save Data. (Use Default SaveData())"));
	}
}

void USTGameInstance::SaveSavedData(const FSaveData& SaveData)
{
	USTSaveGame* SaveObj = Cast<USTSaveGame>(UGameplayStatics::CreateSaveGameObject(USTSaveGame::StaticClass()));

	SaveObj->SaveData = SaveData;

	UGameplayStatics::SaveGameToSlot(SaveObj, USTSaveGame::DefaultSlotName, USTSaveGame::DefaultUserIndex);
	
	UE_LOG(LogSystem, Warning, TEXT("게임 저장 완료! HighScore = %d"), SaveObj->SaveData.HighScore);
}
