// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/STItemBase.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Player/STPlayerCharacter.h"


ASTItemBase::ASTItemBase()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	if (IsValid(TriggerBox))
	{
		RootComponent = TriggerBox;
		TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		TriggerBox->SetCollisionProfileName(TEXT("OverlapAll")); 

		TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ASTItemBase::OnOverlapBegin);
	}

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	if (IsValid(StaticMesh))
	{
		StaticMesh->SetupAttachment(RootComponent);
		StaticMesh->SetCollisionProfileName(FName("NoCollision"));
	}

	PointLight =  CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	if (IsValid(PointLight))
	{
		PointLight->SetupAttachment(RootComponent);
		PointLight->SetIntensity(200.f);
		PointLight->SetAttenuationRadius(200.f);
	}

	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	if (IsValid(ParticleSystemComponent))
	{
		ParticleSystemComponent->SetupAttachment(RootComponent);
		ParticleSystemComponent->bAutoActivate = false; 
	}
	
	

}

void ASTItemBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsOverlapped)
	{
		ASTPlayerCharacter* Player = Cast<ASTPlayerCharacter>(OtherActor);
		if (IsValid(Player))
		{
			UseItem(Player);
		}
	}
	
}

void ASTItemBase::UseItem(ASTPlayerCharacter* InInstigator)
{
	bIsOverlapped  = true;
	if (IsValid(PickupSoundEffect))
	{
		UGameplayStatics::PlaySoundAtLocation(this, PickupSoundEffect, GetActorLocation());
	}
	if (IsValid(PickupParticle))
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickupParticle, GetActorLocation());
	}
	
}

void ASTItemBase::DestroyedItem()
{
	Destroy(); 
}




