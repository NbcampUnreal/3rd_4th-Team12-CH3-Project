// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "STAnimNotify_PlayerFootStep.generated.h"

/**
 * 
 */
UCLASS()
class ST_API USTAnimNotify_PlayerFootStep : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> FootSoundCue;
	UPROPERTY(EditAnywhere)
	float Volume = 0.5f;
	
};
