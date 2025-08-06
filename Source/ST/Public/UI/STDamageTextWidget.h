#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STDamageTextWidget.generated.h"

UCLASS()
class ST_API USTDamageTextWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetDamageText(int32 Damage);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Txt_DamageValue;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	class UWidgetAnimation* Anim_FloatUpAndFade;
};