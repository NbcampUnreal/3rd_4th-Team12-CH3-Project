#pragma once

#include "CoreMinimal.h"
#include "STGameTypes.h"
#include "GameFramework/PlayerState.h"
#include "STPlayerState.generated.h"

/**
 *  플레이어의 개별 정보 저장
 */
UCLASS()
class ST_API ASTPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ASTPlayerState();

	/* getters */
	UFUNCTION( BlueprintPure, Category="PlayerState")
	const FPlayerStateInfo& GetPlayerStateInfo() const;	// 상수 참조를 통해 불필요한 복사 방지

	/* setters */
	void SetSelectedCharacter( const ECharacterType NewCharacter );
	void SetCurrentHP( const float NewHP );
	void SubtractCurrentHP( const float Amount );	// Amound 만큼 빼기
	void SetMaxHP( const float NewHP );
	void SetCurrentAmmo( const int32 NewCurrentAmmo );
	void SubtractCurrentAmmo();				// 1발만 발사
	void SetMaxAmmo( const int32 NewMaxAmmo );
	void AddKillCount();					// 1킬 증가
	void SetKillCount( const int32 NewKillCount );
	void AddScore( const int32 Amount );			// Amount 만큼 점수 증가
	void SetScore( const int32 NewScore );
	void SetHighScore( const int32 NewHighScore );
	void SetCurrWeaponName( const FString& WeaponName );

protected:
	virtual void BeginPlay() override;
	
private:
	FPlayerStateInfo PlayerStateInfo;
	
	
	



	/*
	/*중복선언 에러#1#
	/*UPROPERTY( BlueprintReadWrite, Category="PlayerState" ) 
	int32 Score;#1#

	UPROPERTY( BlueprintReadWrite, Category="PlayerState" )
	int32 KillCount;

	UPROPERTY( BlueprintReadWrite, Category="PlayerState" )
	ECharacterType SelectedCharacter;

	UFUNCTION( BlueprintCallable )
	void ResetPlayerState();*/
};
