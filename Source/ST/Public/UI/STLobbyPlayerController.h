#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "System/STGameTypes.h"
#include "STLobbyPlayerController.generated.h"

class USTLobbyWidget;

UCLASS()
class ST_API ASTLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASTLobbyPlayerController();

	// JM 캐릭터 선택시 바인딩 함수
	UFUNCTION()
	void HandleCharacterSelected(const ECharacterType& CharacterType);

	// JM 메인메뉴로 돌아가기 선택시 바인딩 함수
	UFUNCTION()
	void OnBackToTitleRequested();
	
protected:
	
	virtual void BeginPlay() override;


	UFUNCTION( BlueprintImplementableEvent )
	void StartStage1_BP();

	UFUNCTION( BlueprintImplementableEvent )
	void BackToMainMenu_BP();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<USTLobbyWidget> LobbyWidgetClass;

private:
	UPROPERTY()
	USTLobbyWidget* LobbyWidgetInstance;
};