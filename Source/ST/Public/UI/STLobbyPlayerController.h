#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "STLobbyPlayerController.generated.h"

class USTLobbyWidget;

UCLASS()
class ST_API ASTLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASTLobbyPlayerController();

protected:
	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<USTLobbyWidget> LobbyWidgetClass;

private:
	UPROPERTY()
	USTLobbyWidget* LobbyWidgetInstance;
};