// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/STPlayerCharacter.h"
#include "Player/STPlayerInputConfig.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/STHealthComponent.h"
#include "Player/STMovementComponent.h"
#include "Player/STPlayerAnimInstance.h"
#include "Player/STPlayerBaseData.h"
#include "Player/STWeaponManagerComponent.h"
#include "Player/ST_PlayerAnimMontageConfig.h"
#include "System/STGameMode.h"
#include "System/STPlayerState.h"
#include "Weapon/STWeaponBase.h"
#include "Weapon/STWeaponType.h"

#pragma region DefaultSetting

ASTPlayerCharacter::ASTPlayerCharacter()
{
	// Character Movement Settings
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->bWantsToCrouch = true;
		GetCharacterMovement()->CrouchedHalfHeight = 60.f;
	}
	
	// TPS Camera Setup
	TPSSpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("TPSSpringArmComponent"));
	TPSSpringArmComponent->SetupAttachment(GetRootComponent());
	TPSSpringArmComponent->bUsePawnControlRotation = true;
	TPSSpringArmComponent->bAutoActivate = false;
	TPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TPSCameraComponent"));
	TPSCameraComponent->SetupAttachment(TPSSpringArmComponent, USpringArmComponent::SocketName);
	TPSCameraComponent->bUsePawnControlRotation = false;

	// FPS Camera Setup
	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCameraComponent"));
	FPSCameraComponent->SetupAttachment(GetRootComponent());
	FPSCameraComponent->bUsePawnControlRotation = true;
	FPSCameraComponent->bEnableFirstPersonFieldOfView = true;
	FPSCameraComponent->bEnableFirstPersonScale = true;
	FPSCameraComponent->FirstPersonFieldOfView = FirstPersonFieldOfView;
	FPSCameraComponent->FirstPersonScale = FirstPersonScale;
	FPSCameraComponent->bAutoActivate = false;

	// FPS Mesh Component
	FPSSkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPSSkeletalMeshComponent"));
	FPSSkeletalMeshComponent->SetupAttachment(FPSCameraComponent);
	FPSSkeletalMeshComponent->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FPSSkeletalMeshComponent->SetOnlyOwnerSee(true);
	FPSSkeletalMeshComponent->SetRelativeLocation(FVector(0, 0, -20));
	FPSSkeletalMeshComponent->bAutoActivate = false;

	//HealthCompoennt
	HealthComponent = CreateDefaultSubobject<USTHealthComponent>(TEXT("HealthComponent"));

	//MovementComponent
	MovementComponent = CreateDefaultSubobject<USTMovementComponent>(TEXT("MovementComponent"));
	
	//Weapon Component
	WeaponManager = CreateDefaultSubobject<USTWeaponManagerComponent>(TEXT("WeaponManager"));

}


void ASTPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	//Cached Player State
	if (AController* PC = GetController())
	{
		CachedPlayerState = PC->GetPlayerState<ASTPlayerState>();
		
	}
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC && PC->IsLocalController())
	{
		ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
		if (LocalPlayer)
		{
			UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
			if (Subsystem && InputConfig && InputConfig->DefaultMappingContext)
			{
				Subsystem->AddMappingContext(InputConfig->DefaultMappingContext, 0);
			}
		}
	}
	
	SetViewMode(true); //  TPS view

	if (IsValid(CachedPlayerState))
	{
		if (IsValid(HealthComponent))
		{
			HealthComponent->SetMaxHealth(CachedPlayerState->GetPlayerStateInfo().MaxHP);
			HealthComponent->SetCurrentHealth(CachedPlayerState->GetPlayerStateInfo().CurrentHP);
			HealthComponent->Initialize();
		}
		if (IsValid(MovementComponent))
		{
			MovementComponent->SetWalkSpeed(CachedPlayerState->GetPlayerStateInfo().MoveSpeed);
			MovementComponent->SetCrouchMultiplier(CachedPlayerState->GetPlayerStateInfo().CrouchMultiplier);
			MovementComponent->SetSprintMultiplier(CachedPlayerState->GetPlayerStateInfo().SprintMultiplier);
			MovementComponent->SetZoomMultiplier(CachedPlayerState->GetPlayerStateInfo().ZoomMultiplier);
			MovementComponent->Initialize();
		}
		if (IsValid(WeaponManager))
		{
			TSoftClassPtr<ASTWeaponBase> SoftWeaponClass = CachedPlayerState->GetPlayerStateInfo().PlayerWeaponData.WeaponClass;
			WeaponManager->EquipWeapon(SoftWeaponClass);
		}
	}

	
}

void ASTPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!IsValid(EnhancedInputComponent) || !IsValid(InputConfig))
	{
		return;
	}

	// Bind Input Actions
	if (InputConfig->JumpAction)
	{
		EnhancedInputComponent->BindAction(InputConfig->JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
	}
	if (InputConfig->MoveAction)
	{
		EnhancedInputComponent->BindAction(InputConfig->MoveAction, ETriggerEvent::Triggered, this, &ASTPlayerCharacter::Move);
	}
	if (InputConfig->LookAction)
	{
		EnhancedInputComponent->BindAction(InputConfig->LookAction, ETriggerEvent::Triggered, this, &ASTPlayerCharacter::Look);
	}
	if (InputConfig->SprintAction)
	{
		EnhancedInputComponent->BindAction(InputConfig->SprintAction, ETriggerEvent::Started, this, &ASTPlayerCharacter::Sprint);
		EnhancedInputComponent->BindAction(InputConfig->SprintAction, ETriggerEvent::Completed, this, &ASTPlayerCharacter::EndSprint);
	}
	if (InputConfig->CrouchAction)
	{
		EnhancedInputComponent->BindAction(InputConfig->CrouchAction, ETriggerEvent::Started, this, &ASTPlayerCharacter::Crouch);
	}
	if (InputConfig->ChangeViewAction)
	{
		EnhancedInputComponent->BindAction(InputConfig->ChangeViewAction, ETriggerEvent::Started, this, &ASTPlayerCharacter::ChangeView);
	}
	if (InputConfig->ZoomAction)
	{
		EnhancedInputComponent->BindAction(InputConfig->ZoomAction, ETriggerEvent::Started, this, &ASTPlayerCharacter::Zoom);
	}
	if (InputConfig->FireAction)
	{
		EnhancedInputComponent->BindAction(InputConfig->FireAction, ETriggerEvent::Started, this, &ASTPlayerCharacter::StartFire);
	}
	if (InputConfig->FireAction)
	{
		EnhancedInputComponent->BindAction(InputConfig->FireAction, ETriggerEvent::Completed, this, &ASTPlayerCharacter::StopFire);
	}
	if (InputConfig->ReloadAction)
	{
		EnhancedInputComponent->BindAction(InputConfig->ReloadAction, ETriggerEvent::Completed, this, &ASTPlayerCharacter::ReloadAmmo);
	}

}
#pragma endregion 

#pragma region Camera System

void ASTPlayerCharacter::SetViewMode(bool bIsTPS)
{
	CurrentViewMode = bIsTPS ? EViewMode::TPS : EViewMode::FPS;

	TPSCameraComponent->SetActive(bIsTPS);
	FPSCameraComponent->SetActive(!bIsTPS);
	
	bUseControllerRotationYaw = !bIsTPS;
	bUseControllerRotationPitch = true;
	
	if (bIsTPS)
	{
		GetMesh()->SetOwnerNoSee(false);             
		FPSSkeletalMeshComponent->SetOwnerNoSee(true);  
	}
	else
	{
		
		GetMesh()->SetOwnerNoSee(true);               
		FPSSkeletalMeshComponent->SetOwnerNoSee(false); 
	}
	OnViewModeChanged.Broadcast(CurrentViewMode);
}

#pragma endregion

#pragma region Input System

void ASTPlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D Direction = Value.Get<FVector2D>();
	FRotator Rotation = GetController()->GetControlRotation();
	FRotator YawRotation = FRotator(0, Rotation.Yaw, 0);

	FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardVector, Direction.X);
	AddMovementInput(RightVector, Direction.Y);
}

void ASTPlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D Direction = Value.Get<FVector2D>();
	AddControllerYawInput(Direction.X);
	AddControllerPitchInput(Direction.Y);
}

void ASTPlayerCharacter::Crouch(const FInputActionValue& Value)
{
	if (GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	}
	else
	{
		ACharacter::Crouch();
	}
}



void ASTPlayerCharacter::Sprint(const FInputActionValue& Value)
{

	if (IsValid(MovementComponent))
	{
		MovementComponent->StartSprinting();
	}
	
}

void ASTPlayerCharacter::EndSprint(const FInputActionValue& Value)
{
	if (IsValid(MovementComponent))
	{
		MovementComponent->StopSprinting();
	}
}

void ASTPlayerCharacter::ChangeView(const FInputActionValue& Value)
{
	const bool bNextIsTPS = (CurrentViewMode == EViewMode::FPS);
	SetViewMode(bNextIsTPS);
}

void ASTPlayerCharacter::Zoom(const FInputActionValue& Value)
{
	if (IsValid(MovementComponent))
	{
		MovementComponent->ChangeZoomMode();
		if (IsValid(WeaponManager))
		{
			if (MovementComponent->IsZooming())
			{
				WeaponManager->StartAiming();				
			}
			else
			{
				WeaponManager->StopAiming();
			}
		}
		if (FOnCharacterZooming.IsBound())
		{
			FOnCharacterZooming.Broadcast(MovementComponent->IsZooming());
		}
		
	}
}

void ASTPlayerCharacter::StartFire(const FInputActionValue& Value)
{
	if (IsValid(WeaponManager) && !GetMovementComponent()->IsFalling())
	{
		WeaponManager->StartFire();
	}
}

void ASTPlayerCharacter::StopFire(const FInputActionValue& Value)
{
	if (IsValid(WeaponManager))
	{
		WeaponManager->StopFire();
	}
}
void ASTPlayerCharacter::ReloadAmmo(const FInputActionValue& Value)
{
	if (IsValid(WeaponManager))
	{
		WeaponManager->ReloadAmmo();
	}
}

#pragma endregion

#pragma region Status System

float ASTPlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (IsValid(HealthComponent))
	{
		HealthComponent->TakeDamage(ActualDamage);
	}
	
	return ActualDamage;
}

void ASTPlayerCharacter::HandleDeath()
{
	SetViewMode(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
#pragma endregion

#pragma region WeaponSystem

void ASTPlayerCharacter::OnWeaponFired() 
{
	if (!MontageConfig) return; 
    
	if (CurrentViewMode == EViewMode::FPS)
	{
		
		if (UAnimInstance* AnimInstance = Cast<UAnimInstance>(FPSSkeletalMeshComponent->GetAnimInstance()))
		{
			if (MontageConfig->FPSShootMontage) // null 체크
			{
				AnimInstance->Montage_Play(MontageConfig->FPSShootMontage);
		
			}
		}
	}
}
void ASTPlayerCharacter::PlayReloadAnimation()
{
	if (UAnimInstance* AnimInstance = Cast<UAnimInstance>(FPSSkeletalMeshComponent->GetAnimInstance()))
	{
		if (IsValid(MontageConfig->FPSReloadMontage))
		{
			AnimInstance->Montage_Play(MontageConfig->FPSReloadMontage);
		}
	}
	if (UAnimInstance* AnimInstance = Cast<UAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		if (IsValid(MontageConfig->ReloadMontage))
		{
			AnimInstance->Montage_Play(MontageConfig->ReloadMontage);
		}
	}
}
#pragma endregion




