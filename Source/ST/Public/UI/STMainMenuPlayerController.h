#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "STMainMenuPlayerController.generated.h"

class USTMainMenuWidget;

UCLASS()
class ST_API ASTMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASTMainMenuPlayerController();

	/** 메인 메뉴 UI 클래스 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<USTMainMenuWidget> MainMenuWidgetClass;

	UFUNCTION()
	void HandleNewGameRequested();	// JM: 새 게임시작 버튼 Onclick 함수(STMainMenuWidget에서 직접호출)

	UFUNCTION()
	void HandleQuitRequested();		// JM: 게임종료 버튼 Onclick 함수(STMainMenuWidget에서 직접호출)
	
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION( BlueprintImplementableEvent )
	void LoadLobbyLevel_BP();	// JM: Loading 화면 띄우기(BP Only) -> 레벨이동

private:
	UPROPERTY()
	USTMainMenuWidget* MenuInstance;
};