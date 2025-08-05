// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/STPlayerCharacter.h"
#include "Player/STPlayerInputConfig.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Player/STStatusComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#pragma region DefaultSetting
ASTPlayerCharacter::ASTPlayerCharacter()
{
	
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

	// Status Component
	StatusComponent = CreateDefaultSubobject<USTStatusComponent>(TEXT("StatusComponent"));

	// Character Movement Settings
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetCharacterMovement()->bWantsToCrouch = true;
		GetCharacterMovement()->CrouchedHalfHeight = 60.f;
	}

}


void ASTPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
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
	
	if (GetCharacterMovement() && IsValid(StatusComponent))
	{
		GetCharacterMovement()->MaxWalkSpeed = StatusComponent->GetMoveSpeed();
		GetCharacterMovement()->MaxWalkSpeedCrouched = StatusComponent->GetCrouchSpeed();
	}
	SetViewMode(true); //  TPS view
	
	// Bind Status Component Events
	if (IsValid(StatusComponent))
	{
		StatusComponent->OnHealthChanged.AddDynamic(this, &ASTPlayerCharacter::HandleTakeDamage);
		StatusComponent->OnCharacterDeath.AddDynamic(this, &ASTPlayerCharacter::HandleDeath);
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

	// 메시 가시성 제어
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
}

#pragma endregion


#pragma region Input System

void ASTPlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D Direction = Value.Get<FVector2D>();
	FRotator Rotation = GetController()->GetControlRotation();
	FRotator YawRotation = FRotator(0, Rotation.Yaw, 0);

	FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	FVector RightVector = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);

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
	if (!IsValid(StatusComponent) || StatusComponent->IsSprinting())
	{
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Sprinting"));
	StatusComponent->SetSprinting(true);
	GetCharacterMovement()->MaxWalkSpeed = StatusComponent->GetMoveSpeed();
}

void ASTPlayerCharacter::EndSprint(const FInputActionValue& Value)
{
	if (IsValid(StatusComponent) && StatusComponent->IsSprinting())
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("EndSprinting"));
		StatusComponent->SetSprinting(false);
		GetCharacterMovement()->MaxWalkSpeed = StatusComponent->GetMoveSpeed();
	}
}

void ASTPlayerCharacter::ChangeView(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("ChangeView"));
	const bool bNextIsTPS = (CurrentViewMode == EViewMode::FPS);
	SetViewMode(bNextIsTPS);
}

void ASTPlayerCharacter::Zoom(const FInputActionValue& Value)
{
	if (IsValid(StatusComponent))
	{
		StatusComponent->SetZoom(!StatusComponent->IsZooming());
		GetCharacterMovement()->MaxWalkSpeed = StatusComponent->GetMoveSpeed();
	}
}


#pragma endregion

#pragma region Status System


float ASTPlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (IsValid(StatusComponent))
	{
		StatusComponent->TakeDamage(ActualDamage);
	}
	
	return ActualDamage;
}

void ASTPlayerCharacter::HandleTakeDamage(float InNewHp, float InMaxHp)
{
	// TODO:
	// A. Hit Montage
	// B. Request UI Change
}

void ASTPlayerCharacter::HandleDeath()
{
	// TODO:
	// A. Death Montage
	// B. Change Collision
	// C. Show Death UI
}


#pragma endregion 





