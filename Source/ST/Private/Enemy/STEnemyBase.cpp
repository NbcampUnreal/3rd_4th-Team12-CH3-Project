// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/STEnemyBase.h"

// Sets default values
ASTEnemyBase::ASTEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASTEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASTEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASTEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

