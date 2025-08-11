// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/STAnimNotify_PlayerFootStep.h"
#include "Perception/AISense_Hearing.h"
#include "Kismet/GameplayStatics.h"

void USTAnimNotify_PlayerFootStep::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (!MeshComp) return;

	UWorld* World = MeshComp->GetWorld();
	if (!World) return;

	FVector Location = MeshComp->GetComponentLocation();
	UGameplayStatics::PlaySoundAtLocation(World, FootSoundCue, Location , Volume);
	UAISense_Hearing::ReportNoiseEvent(
	World,
	Location,   
	1.0f,                    
	MeshComp->GetOwner()
	);
	UE_LOG(LogTemp, Warning, TEXT("FootstepSound is sound!"));
	
}
