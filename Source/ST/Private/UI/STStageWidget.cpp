#include "UI/STStageWidget.h"
#include "UI/STDamageTextWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "TimerManager.h"
#include "System/STGameState.h"
#include "System/STLog.h"
#include "UI/STDamageLogUIWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void USTStageWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Img_HitMarker)
        Img_HitMarker->SetVisibility(ESlateVisibility::Hidden);

    if (Img_KillConfirmed)
        Img_KillConfirmed->SetVisibility(ESlateVisibility::Hidden);
}

void USTStageWidget::UpdateHealth(float CurrentHP, float MaxHP)
{
    if (PB_Health)
        PB_Health->SetPercent(CurrentHP / FMath::Max(MaxHP, 1.f));

    if (Txt_HealthValue)
    {
        FString Text = FString::Printf(TEXT("%d / %d"), FMath::RoundToInt(CurrentHP), FMath::RoundToInt(MaxHP));
        Txt_HealthValue->SetText(FText::FromString(Text));
    }
}

void USTStageWidget::UpdateWeapon(const FText& WeaponName)
{
    if (Txt_WeaponType)
    {
        Txt_WeaponType->SetText(WeaponName);
    }
}

void USTStageWidget::UpdateAmmo(int32 CurrentAmmo, int32 MaxAmmo)
{
    if (Txt_AmmoInfo)
    {
        FString Text = FString::Printf(TEXT("%d / %d"), CurrentAmmo, MaxAmmo);
        Txt_AmmoInfo->SetText(FText::FromString(Text));
    }
}

void USTStageWidget::UpdateTimer(int32 SecondsRemaining)
{
    int32 Minutes = SecondsRemaining / 60;
    int32 Seconds = SecondsRemaining % 60;

    if (Txt_Timer)
    {
        FString Text = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
        Txt_Timer->SetText(FText::FromString(Text));
    }
}

void USTStageWidget::UpdateEnemyStatus(int32 Killed, int32 Total)
{
    if (Txt_EnemyStatus)
    {
        FString Text = FString::Printf(TEXT("%d / %d"), Killed, Total);
        Txt_EnemyStatus->SetText(FText::FromString(Text));
    }
}

void USTStageWidget::AddDamageKillLog(const FString& LogText)
{
    if (!DamageKillLogBox || !DamageLogItemClass) return;

    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;

    USTDamageLogUIWidget* LogItem = CreateWidget<USTDamageLogUIWidget>(PC, DamageLogItemClass);
    if (!LogItem) return;

    // 텍스트 설정
    LogItem->SetText(LogText);

    // 상단에 추가
    DamageKillLogBox->AddChildToVerticalBox(LogItem);

    // 최대 5개 유지
    while (DamageKillLogBox->GetChildrenCount() > 5)
    {
        DamageKillLogBox->RemoveChildAt(0);
    }

    // 페이드 아웃 후 제거
    LogItem->PlayFadeAndDestroy();
}

void USTStageWidget::ShowHitMarker()
{
    if (!Img_HitMarker)
    {
        return;
    }

    GetWorld()->GetTimerManager().ClearTimer(HitMarkerTimerHandle);

    Img_HitMarker->SetRenderOpacity(1.f);
    Img_HitMarker->SetVisibility(ESlateVisibility::Visible);

    GetWorld()->GetTimerManager().SetTimer(
        HitMarkerTimerHandle, this, &USTStageWidget::HideHitMarker, 0.15f, false);
    
}

void USTStageWidget::HideHitMarker()
{
    if (Img_HitMarker)
        Img_HitMarker->SetVisibility(ESlateVisibility::Hidden);
}

void USTStageWidget::ShowKillConfirmedImage()
{
    if (Img_KillConfirmed)
    {
        Img_KillConfirmed->SetVisibility(ESlateVisibility::Visible);
        GetWorld()->GetTimerManager().SetTimer(KillConfirmedTimerHandle, this, &USTStageWidget::HideKillConfirmedImage, 1.5f, false);
    }
}

void USTStageWidget::HideKillConfirmedImage()
{
    if (Img_KillConfirmed)
        Img_KillConfirmed->SetVisibility(ESlateVisibility::Hidden);
}

void USTStageWidget::ShowDamageTextAt(FVector WorldLocation, int32 Damage)
{
    ShowDamageTextAtEx(WorldLocation, Damage, false);
}

void USTStageWidget::ShowDamageTextAtEx(FVector WorldLocation, int32 Damage, bool bCritical)
{
    if (!STDamageTextWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ShowDamageTextAtEx: STDamageTextWidgetClass NOT set."));
        return;
    }

    APlayerController* PC = GetOwningPlayer();
    if (!PC) { PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr; }
    if (!PC) { UE_LOG(LogTemp, Error, TEXT("ShowDamageTextAtEx: No PlayerController.")); return; }

    FVector2D ViewportPos;
    const bool bOK = UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
        PC, WorldLocation, ViewportPos, false);
    if (!bOK)
    {
        UE_LOG(LogTemp, Warning, TEXT("ShowDamageTextAtEx: ProjectWorldLocationToWidgetPosition failed. WL=%s"), *WorldLocation.ToString());
        return;
    }

    USTDamageTextWidget* W = CreateWidget<USTDamageTextWidget>(PC, STDamageTextWidgetClass);
    if (!W) { UE_LOG(LogTemp, Error, TEXT("ShowDamageTextAtEx: CreateWidget failed.")); return; }
    
    W->AddToViewport(2000);
    W->SetVisibility(ESlateVisibility::HitTestInvisible);
    W->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
    W->SetPositionInViewport(ViewportPos, /*bRemoveDPIScale=*/false);
    
    W->SetDamage(Damage, bCritical);
    W->SetRenderOpacity(1.f);
}