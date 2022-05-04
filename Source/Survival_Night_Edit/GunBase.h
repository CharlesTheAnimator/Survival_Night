// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/DamageType.h"
#include "BaseProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h" 
#include "GunBase.generated.h"

UCLASS()
class SURVIVAL_NIGHT_EDIT_API AGunBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGunBase();

	// Called when player pulls trigger
	void FireGun(bool bDebug = false);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
	TSubclassOf<class ABaseProjectile> ProjectileClass;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Gun muzzle offset from the camera location.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	UPROPERTY(EditAnywhere)			//Muzzle Particle Effect Component
	UParticleSystem* MuzzleFlash;
	UPROPERTY(EditAnywhere)			//Impact Particle system to spawn
	UParticleSystem* BulletImpact;
	UPROPERTY(EditAnywhere)			//Muzzle flash location offset Vector at gun fire call
	FVector MuzzleLocationOffset;
	UPROPERTY(EditAnywhere)			//Muzzle flash rotation offset Vector at gun fire call
	FRotator MuzzleRotationOffset;
	UPROPERTY(EditAnywhere)			//Muzzle Flash Scale
	FVector MuzzleFlashScale;

private:
	//USed To set up Gun Mesh
	UPROPERTY(VisibleAnywhere)		//Root Component
	USceneComponent* Root;
	UPROPERTY(VisibleAnywhere)		//Mesh Component
	USkeletalMeshComponent* Mesh;

	//Gun GameStat Properties
	UPROPERTY(EditAnywhere)
	float MaxRange = 1000.0f;
	UPROPERTY(EditAnywhere)
	float BulletSpawnOffset = 50.0f;
	UPROPERTY(EditAnywhere)
	float BulletPitch = 5.0f;
};
