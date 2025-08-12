﻿#include "System/StageClearZone.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Player/STPlayerCharacter.h"
#include "System/STGameMode.h"
#include "System/STLog.h"

/************ public ************/
AStageClearZone::AStageClearZone()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	RootComponent = Scene;		// 생성자에서 루트 지정시
	// SetRootComponent(Scene); // 런타임에 루트 변경시

	Trigger = CreateDefaultSubobject<USphereComponent>("Trigger");
	Trigger->SetCollisionProfileName("Trigger");
	Trigger->SetupAttachment(Scene);
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetupAttachment(Scene);
	StaticMesh->SetSimulatePhysics(false);
}

/************ protected ************/
void AStageClearZone::BeginPlay()
{
	Super::BeginPlay();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AStageClearZone::OnOverlapBegin);	
}

void AStageClearZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogSystem, Log, TEXT("AStageClearZone::OnOverlapBegin() Start"));
	if (OtherActor && OtherActor->IsA(ASTPlayerCharacter::StaticClass()))	// TODO: AI가 들어갔을때 발동되진 않는지 테스트
	{
		UE_LOG(LogSystem, Log, TEXT("AStageClearZone::OnOverlapBegin() Call Delegate(OnPlayerEnteredClearZone)"));
		OnPlayerEnteredClearZone.Broadcast();
	}
	UE_LOG(LogSystem, Log, TEXT("AStageClearZone::OnOverlapBegin() End"));
}


