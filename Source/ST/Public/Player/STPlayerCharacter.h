// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "STPlayerCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCharacterZooming, bool);




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

// Forward Declarations
class UCameraComponent;
class USpringArmComponent;
struct FInputActionValue;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnViewModeChanged, EViewMode); // when view Change

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
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
    TObjectPtr<USpringArmComponent> TPSSpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
    TObjectPtr<UCameraComponent> TPSCameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Camera")
    TObjectPtr<UCameraComponent> FPSCameraComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float FirstPersonFieldOfView = 103.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
    float FirstPersonScale = 1.0f;

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

    void OnDeathMontageEnded(UAnimMontage* Montage, bool bInterrupted);
public:
    USTMovementComponent* GetPlayerMovementComponent() const {return MovementComponent;}
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component|Movement")
    TObjectPtr<USTMovementComponent> MovementComponent;
#pragma  endregion

#pragma region Weapon System

public:
    void OnWeaponEquipped(EWeaponType NewWeapon);
    void OnWeaponFired();
    void PlayReloadAnimation();
protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
    TObjectPtr<USTWeaponManagerComponent> WeaponManager;
    
#pragma endregion

#pragma region Config

    // Input Config
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USTPlayerInputConfig> InputConfig;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UST_PlayerAnimMontageConfig> MontageConfig;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<USTPlayerBaseData> PlayerBaseStatData;
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
