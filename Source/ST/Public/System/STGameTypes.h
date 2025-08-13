﻿#pragma once

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
	Stage1		UMETA( DisplayName = "Stage1" ),
	Stage2		UMETA( DisplayName = "Stage2" ),
	Stage3		UMETA( DisplayName = "Stage3" ),
	Ending		UMETA( DisplayName = "Ending" ),
};

USTRUCT( BlueprintType )
struct FPlayerStateInfo		// TODO: 향후 PlayerState가 커지면, 레벨 이동간 유지해야할 정보만 따로 뺄 수도 있음
{
	GENERATED_BODY()

	FPlayerStateInfo()
		: SelectedCharacter(ECharacterType::None)
		, CurrentHP(100.0f)
		, MaxHP(100.0f)
		, CurrentAmmo(0)
		, MaxAmmo(0)
		, KillCount(0)
		, Score(0)
		, HighScore(0)
		, CurrWeaponName(TEXT("NONE"))
		, TotalDamageReceived(0.0f)
		, TotalDamageInflicted(0.0f)
		, TotalUsedAmmo(0)
	{}

	ECharacterType SelectedCharacter;
	float CurrentHP;
	float MaxHP;
	int32 CurrentAmmo;
	int32 MaxAmmo;
	int32 KillCount;
	int32 Score;
	int32 HighScore;
	FString CurrWeaponName;	// TODO: Enum으로 확장 필요하겠는데
	float TotalDamageReceived;	// 받은 피해량
	float TotalDamageInflicted;	// 가한 피해량
	int32 TotalUsedAmmo;
};

USTRUCT( BlueprintType )
struct FGameStateInfo
{
	GENERATED_BODY()

	FGameStateInfo()
		: StagePhase(EStagePhase::Start)
		, StageResult(EStageResult::None)
		, RemainingEnemies(0)
		, RemainingTime(0.0f)
		, BossPhase(1)
		, StageGoalText(FText::GetEmpty())
	{};

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "GameState" )
	EStagePhase StagePhase;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "GameState" )
	EStageResult StageResult;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "GameState" )
	int32 RemainingEnemies;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "GameState" )
	int32 RemainingTime;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "GameState" )
	int32 BossPhase;
	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "GameState" )
	FText StageGoalText;
};