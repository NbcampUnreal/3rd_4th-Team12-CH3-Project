#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STEnemyProjectile.generated.h"

UCLASS()
class ST_API ASTEnemyProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ASTEnemyProjectile();

	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
	class USphereComponent* CollisionComponent;
	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	class UStaticMeshComponent* ProjectileMesh;
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	class UProjectileMovementComponent* ProjectileMovement;

	void FireInDirection(const FVector& ShootDirection);

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnHit(
		UPrimitiveComponent* HitComp, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		FVector NormalImpulse, 
		const FHitResult& Hit);
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
};
