// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "STIItem.h"
#include "GameFramework/Actor.h"
#include "STItemBase.generated.h"

class UPointLightComponent;
class UBoxComponent;

UCLASS()
class ST_API ASTItemBase : public AActor, public ISTIItem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASTItemBase();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> TriggerBox;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPointLightComponent> PointLight;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UParticleSystemComponent>  ParticleSystemComponent;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> PickupSoundEffect;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> PickupParticle;



	

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	virtual void UseItem(ASTPlayerCharacter* InInstigator)override;

	virtual void DestroyedItem();
	
private:
	bool bIsOverlapped = false;
};
