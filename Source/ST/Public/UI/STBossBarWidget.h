// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STBossBarWidget.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class ST_API USTBossBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category="BossBar")
	void SetBossName(const FText& InName);

	UFUNCTION(BlueprintCallable, Category="BossBar")
	void UpdateBossHP(float CurrentHP, float MaxHP);

protected:
	UPROPERTY(meta = (BindWidget)) UTextBlock* Txt_BossName = nullptr;
	UPROPERTY(meta = (BindWidget)) UProgressBar* PB_BossHealth = nullptr;
};
