#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STDamageTextWidget.generated.h"

UCLASS()
class ST_API USTDamageTextWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	

	UFUNCTION(BlueprintCallable) void SetDamageText(int32 Damage);
	UFUNCTION(BlueprintCallable) void SetDamage(int32 Damage, bool bCritical);
	
protected:
	UPROPERTY(meta = (BindWidget)) class UTextBlock* Txt_DamageValue;
	UPROPERTY(Transient, meta = (BindWidgetAnim)) class UWidgetAnimation* Anim_FloatUpAndFade;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DamageText|Style") FLinearColor NormalColor = FLinearColor::White;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DamageText|Style") FLinearColor CriticalColor = FLinearColor(1.f,0.3f,0.2f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DamageText|Style") float NormalScale = 1.0f;   // SH
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DamageText|Style") float CriticalScale = 1.3f; // SH

private:
	UFUNCTION() void OnFloatUpAndFadeFinished();
};