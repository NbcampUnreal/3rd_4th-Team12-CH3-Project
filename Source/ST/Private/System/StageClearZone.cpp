#include "System/StageClearZone.h"
#include "Components/SphereComponent.h"
#include "Player/STPlayerCharacter.h"
#include "System/STLog.h"

/************ public ************/
AStageClearZone::AStageClearZone()
{
	PrimaryActorTick.bCanEverTick = false;

	// Root Component
	Scene = CreateDefaultSubobject<USceneComponent>("Scene");
	RootComponent = Scene;		// 생성자에서 루트 지정시
	// SetRootComponent(Scene); // 런타임에 루트 변경시

	// Trigger Component
	Trigger = CreateDefaultSubobject<USphereComponent>("Trigger");
	Trigger->SetCollisionProfileName("Trigger");
	Trigger->SetupAttachment(Scene);

	// Static Mesh
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
	
	if (OtherActor && OtherActor->IsA(ASTPlayerCharacter::StaticClass()))
	{
		UE_LOG(LogSystem, Log, TEXT("AStageClearZone::OnOverlapBegin() Call Delegate(OnPlayerEnteredClearZone)"));
		OnPlayerEnteredClearZone.Broadcast();
	}
	else
	{
		UE_LOG(LogSystem, Warning, TEXT("AStageClearZone::OnOverlapBegin() Actor isn't STPlayer Character"));
	}
	
	UE_LOG(LogSystem, Log, TEXT("AStageClearZone::OnOverlapBegin() End"));
}


