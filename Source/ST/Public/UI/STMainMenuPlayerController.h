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

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	USTMainMenuWidget* MenuInstance;
};