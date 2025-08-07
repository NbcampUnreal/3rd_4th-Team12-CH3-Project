#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "STGameTypes.generated.h"

/**
 *	Enum 모음집
 */

UENUM( BlueprintType )
enum class EStagePhase : uint8
{
	Start		UMETA( DisplayName = "Start" ),		// 스테이지 시작 직후(인트로, 준비 등)
	InProgress	UMETA( DisplayName = "InProgress" ),// 진행 중(적 전투, 탐험 등)
	AllKilled	UMETA( DisplayName = "AllKilled" ),	// 모든 적을 섬멸한 상태 (목적지로 이동 필요)
	Boss		UMETA( DisplayName = "Boss" ),		// 보스 페이즈 진입
	Completed	UMETA( DisplayName = "Completed" ),	// 스테이지 클리어
	Fail		UMETA( DisplayName = "Fail" ),		// 실패 (타임오버/사망 등)
};

UENUM( BlueprintType )
enum class EStageResult : uint8
{
	None	UMETA( DisplayName = "None" ),	// 아직 결과 없음(진행 중)
	Clear	UMETA( DisplayName = "Clear" ),	// 스테이지 성공
	Fail    UMETA( DisplayName = "Fail" ),	// 스테이지 실패
};

UENUM( BlueprintType )
enum class ECharacterType : uint8
{
	None		UMETA( DisplayName = "None" ),
	JaxMercer	UMETA( DisplayName = "JaxMercer" ),
	AvaRaines	UMETA( DisplayName = "AvaRaines" ),
};

UENUM( BlueprintType )
enum class EStageType : uint8
{
	None		UMETA( DisplayName = "None" ),
	MainMenu	UMETA( DisplayName = "MainMenuLevel" ),
	Lobby		UMETA( DisplayName = "LobbyLevel" ),
	Stage1		UMETA( DisplayName = "L_Showcase" ),
	Stage2		UMETA( DisplayName = "StylizedCyberpunk" ),
	Stage3		UMETA( DisplayName = "Demonstration" ),
	Ending		UMETA( DisplayName = "Ending" ),
};