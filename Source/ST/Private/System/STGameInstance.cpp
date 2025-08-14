#include "System/STGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Player/STPlayerBaseData.h"
#include "System/STLog.h"

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
	ResetPlayerStateInfo();
}

/*void USTGameInstance::Init()
{
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::Init() Start"));
	Super::Init();

	if (MainPlayerClass)	// 게임 시작시 한 번만 캐릭터 인스턴스 생성 및 보관
	{
		PlayerPawnInstance = GetWorld()->SpawnActor<APawn>(MainPlayerClass);
		if (PlayerPawnInstance)
		{
			UE_LOG(LogSystem, Log, TEXT("USTGameInstance::Init() Set PlayerPawnInstance"));
			PlayerPawnInstance->SetActorHiddenInGame(true);
			PlayerPawnInstance->SetActorEnableCollision(false);
			PlayerPawnInstance->SetActorTickEnabled(false);
		}
	}
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::Init() End"));
}*/


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

	// TODO: 일일히 변수 하나씩 하지말고 struct로 확장필요
	LastStage = EStageType::None;
	SelectedCharacter = ECharacterType::None;
	UE_LOG(LogSystem, Log, TEXT("USTGameInstance::ResetGameData() End"));
}

void USTGameInstance::ResetPlayerStateInfo() // player 정보 초기화 
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
