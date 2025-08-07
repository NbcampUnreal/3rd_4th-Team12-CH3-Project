#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STStageWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UVerticalBox;
class UImage;
class USTDamageTextWidget;
class USTDamageLogUIWidget;

UCLASS()
class ST_API USTStageWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // UI 업데이트 함수
    UFUNCTION(BlueprintCallable) void UpdateHealth(float CurrentHP, float MaxHP);
    UFUNCTION(BlueprintCallable) void UpdateWeapon(const FString& WeaponName);
    UFUNCTION(BlueprintCallable) void UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo);
    UFUNCTION(BlueprintCallable) void UpdateTimer(int32 SecondsRemaining);
    UFUNCTION(BlueprintCallable) void UpdateEnemyStatus(int32 Killed, int32 Total);
    UFUNCTION(BlueprintCallable) void AddDamageKillLog(const FString& LogText);

    // 전투 피드백
    UFUNCTION(BlueprintCallable) void ShowHitMarker();
    UFUNCTION(BlueprintCallable) void ShowDamageTextAt(FVector WorldLocation, int32 Damage);
    UFUNCTION(BlueprintCallable) void ShowKillConfirmedImage();

protected:
    virtual void NativeConstruct() override;

    // UMG 바인딩 위젯들
    UPROPERTY(meta = (BindWidget)) UProgressBar* PB_Health;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Txt_HealthValue;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Txt_WeaponType;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Txt_AmmoInfo;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Txt_Timer;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Txt_EnemyStatus;
    UPROPERTY(meta = (BindWidget)) UVerticalBox* DamageKillLogBox;
    UPROPERTY(meta = (BindWidget)) UImage* Img_Crosshair;

    UPROPERTY(meta = (BindWidget)) UImage* Img_HitMarker;
    UPROPERTY(meta = (BindWidget)) UImage* Img_KillConfirmed;

    // 데미지 텍스트 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI|Damage")
    TSubclassOf<USTDamageTextWidget> STDamageTextWidgetClass;

    // 데미지 로그 위젯 클래스
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<USTDamageLogUIWidget> DamageLogItemClass;

private:
    FTimerHandle HitMarkerTimerHandle;
    FTimerHandle KillConfirmedTimerHandle;

    void HideHitMarker();
    void HideKillConfirmedImage();
};