// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "STPlayerCharacter.generated.h"

class ASTWeaponBase;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterZooming, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnViewModeChanged, EViewMode); // when view Change

// Forward Declarations
class UCameraComponent;
class USpringArmComponent;
class USTCameraComponent;
struct FInputActionValue;
class USTPlayerBaseData;
class USTMovementComponent;
class USTHealthComponent;
class USTPlayerAnimInstance;
enum class EWeaponType : uint8;
class UST_PlayerAnimMontageConfig;
class USTWeaponManagerComponent;
class USTStatusComponent;
class USTPlayerInputConfig;
class ASTPlayerState;

UENUM(BlueprintType)
enum class EViewMode : uint8
{
	TPS UMETA(DisplayName = "TPS"),
	FPS UMETA(DisplayName = "FPS"),
};

UCLASS()
class ST_API ASTPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
    ASTPlayerCharacter();
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    virtual void BeginPlay() override;

#pragma region SkeletalMesh 

public:
    USkeletalMeshComponent* GetTPSSkeletalMesh(){return GetMesh();}
    USkeletalMeshComponent* GetFPSSkeletalMesh(){return FPSSkeletalMeshComponent;}
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FPS Mesh")
    TObjectPtr<USkeletalMeshComponent> FPSSkeletalMeshComponent;
    

#pragma endregion 

#pragma region Camera System

public:
    EViewMode GetCurrentViewMode() const { return CurrentViewMode; }
    FOnViewModeChanged OnViewModeChanged;
protected:
    // Camera Functions
    void SetViewMode(bool bIsTPS);

    // Camera Components
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera|TPSCamera")
    TObjectPtr<USpringArmComponent> TPSSpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera|TPSCamera")
    TObjectPtr<USTCameraComponent> TPSCameraComponent;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|TPSCamera")
    float TPSNormalSpringArmTargetLength = 300.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|TPSCamera")
    float TPSZoomSpringArmTargetLength = 150.f;
  
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|TPSCamera")
    float TPSNormalFOV = 90.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|TPSCamera")
    float TPSZoomFOV = 60.f;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera|FPSCamera")
    TObjectPtr<USTCameraComponent> FPSCameraComponent;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|FPSCamera")
    float FPSFieldOfView = 105.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|FPSCamera")
    float FPSZoomFieldOfView = 75.f;

    // Camera Variables
    EViewMode CurrentViewMode = EViewMode::TPS;
#pragma endregion

#pragma region Input System
protected:
    // Input Action Functions
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void Crouch(const FInputActionValue& Value);
    void Sprint(const FInputActionValue& Value);
    void EndSprint(const FInputActionValue& Value);
    void ChangeView(const FInputActionValue& Value);
    void Zoom(const FInputActionValue& Value);
    void StartFire(const FInputActionValue& Value);
    void StopFire(const FInputActionValue& Value);
    void ReloadAmmo(const FInputActionValue& Value);

#pragma endregion

#pragma region Component
//health
public:
    USTHealthComponent* GetHealthComponent() const { return HealthComponent; }
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component|Health")
    TObjectPtr<USTHealthComponent> HealthComponent;
    
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    UFUNCTION()
    void HandleDeath();

public:
    USTMovementComponent* GetPlayerMovementComponent() const {return MovementComponent;}
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component|Movement")
    TObjectPtr<USTMovementComponent> MovementComponent;
#pragma  endregion

#pragma region Weapon System

public:
    USTWeaponManagerComponent* GetWeaponManagerComponent()const {return WeaponManager;}
    void OnWeaponFired();
    void PlayReloadAnimation();
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
    TObjectPtr<USTWeaponManagerComponent> WeaponManager;
    
#pragma endregion

#pragma region Config

public:
    UST_PlayerAnimMontageConfig* GetMontageConfig()const{return MontageConfig;}
protected:
    // Input Config
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USTPlayerInputConfig> InputConfig;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UST_PlayerAnimMontageConfig> MontageConfig;
#pragma endregion

#pragma region Delegate
public:
    FOnCharacterZooming FOnCharacterZooming;
#pragma endregion

#pragma region PlayerState
protected:
    TObjectPtr<ASTPlayerState> CachedPlayerState;
#pragma endregion 
};
