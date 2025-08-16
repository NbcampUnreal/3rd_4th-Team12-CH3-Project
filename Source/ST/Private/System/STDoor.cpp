#include "System/STDoor.h"

#include "Enemy/STEnemyBase.h"
#include "Enemy/STEnemyBoss.h"
#include "Kismet/GameplayStatics.h"
#include "System/STLog.h"


ASTDoor::ASTDoor()
{
	PrimaryActorTick.bCanEverTick = false;
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>("DoorMesh");
	RootComponent = DoorMesh;

	bIsClosed = false;
}

void ASTDoor::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASTEnemyBase::StaticClass(), Enemies);
	for (AActor* E : Enemies)
	{
		if (ASTEnemyBoss* Boss = Cast<ASTEnemyBoss>(E))
		{
			Boss->OnDied.AddDynamic(this, &ASTDoor::HandleBossDefeated);
			UE_LOG(LogSystem, Warning, TEXT("ASTDoor::BeginPlay() Binding Success : HandleBossDefeated"));
		}
		else
		{
			UE_LOG(LogSystem, Warning, TEXT("ASTDoor::BeginPlay() Binding Failed : HandleBossDefeated"));
		}
	}
}

void ASTDoor::HandleBossDefeated(AActor* DeadEnemy)
{
	
	if (Cast<ASTEnemyBoss>(DeadEnemy))
	{
		if (bIsClosed)
		{
			OpenDoor();
			bIsClosed = false;
			UE_LOG(LogSystem, Warning, TEXT("ASTDoor::HandleBossDefeated() Door Opened"));
		}
	}
	else
	{
		UE_LOG(LogSystem, Warning, TEXT("ASTDoor::HandleBossDefeated() Cast Failed"));
		
	}
}