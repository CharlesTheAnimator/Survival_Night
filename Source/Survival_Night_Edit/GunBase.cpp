// Fill out your copyright notice in the Description page of Project Settings.


#include "GunBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"
#include "CollisionQueryParams.h"
#include "Kismet/GameplayStatics.h"
#include "BaseProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/World.h"

// Sets default values
AGunBase::AGunBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Creates Static mesh Subobject component and attaches it to root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	
	// Links mesh to root component
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);

	// Initialized Values Base Gun class
	MuzzleFlashScale.Set(0.01f, 0.01f, 0.01f);
	MuzzleLocationOffset.Set(0.0f, 0.0f, 0.5f);
	MuzzleRotationOffset = Mesh->GetSocketRotation(TEXT("MuzzleSokcet"));


}

// Called when the game starts or when spawned
void AGunBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGunBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called on Gun fire
void AGunBase::FireGun(bool bDebug)
{

	//FVector Constructor for Aim Location and Rotation
	FVector Location;
	FRotator Rotation;
	FVector MuzzleLocation = Mesh->GetSocketLocation(TEXT("MuzzleSokcet"));

	//Get hold of Owner pawn possessing the gun
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;

	//Get Hold of Controller of Owner Pawn
	AController* OwnerController = OwnerPawn->GetController();
	if (OwnerController == nullptr) return;

	//Get Player's ViewPoint
	OwnerController->GetPlayerViewPoint(
		Location,
		Rotation
	);
	FVector EndPoint = Location + Rotation.Vector() * MaxRange;

	//Hit Target EStruct
	FHitResult HitTarget;

	//Set Collision Parameters
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(GetOwner());

	//Check if Bullet line collides with any target
	GetWorld()->LineTraceSingleByChannel(
		HitTarget,
		Location,
		EndPoint,
		ECollisionChannel::ECC_GameTraceChannel11,
		Params
	);

	//Check Bullet for Collision
	/*NOTE: This method is two part. 1) it check if for an actor between muzzle of gun & Location 
	player is aiming. 2) Then re-draws a new ray-trace from muzzle to target location*/
	if (HitTarget.GetActor() != nullptr) {
		EndPoint = HitTarget.Location;
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			BulletImpact,
			HitTarget.Location
		);
	}
	GetWorld()->LineTraceSingleByChannel(
		HitTarget,
		MuzzleLocation,
		EndPoint,
		ECollisionChannel::ECC_GameTraceChannel11,
		Params
	);

	//Called to Spawn Muzzle Particle Effect
	UGameplayStatics::SpawnEmitterAttached(
		MuzzleFlash,
		Mesh,
		"MuzzleSocket",
		MuzzleLocationOffset,
		MuzzleRotationOffset,
		MuzzleFlashScale
	);

	// Attempt to fire a projectile.
	if (ProjectileClass)
	{
		// Get the camera transform.
		FVector CameraLocation;
		FRotator CameraRotation;
		GetActorEyesViewPoint(CameraLocation, CameraRotation);

		// Set MuzzleOffset to spawn projectiles slightly in front of the camera.
		// NOTE: Final LaunchDirection is inversed with a *-1 :: Place all adjustments 
		MuzzleOffset.Set((-1.0f*BulletSpawnOffset), 0.0f, 0.0f);

		// Transform MuzzleOffset from camera space to world space.
		MuzzleLocation = MuzzleLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);

		// Skew the aim to be slightly upwards.
		FRotator MuzzleRotation = CameraRotation;
		MuzzleRotation.Pitch -= BulletPitch;

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			// Spawn the projectile at the muzzle.
			ABaseProjectile* Projectile = World->SpawnActor<ABaseProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
			if (Projectile)
			{
				// Set the projectile's initial trajectory.
				FVector LaunchDirection = (MuzzleRotation.Vector())*-1;
				Projectile->FireInDirection(LaunchDirection);
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Debug Check
	if (bDebug) {
		//Debug Camera
		DrawDebugCamera(
			GetWorld(),
			Location,
			Rotation,
			90,
			1.0f,
			FColor::Red,
			true
		);
		//Debug aim point
		DrawDebugPoint(
			GetWorld(),
			EndPoint,
			20,
			FColor::Red,
			true
		);
		//Debug Bullet Fx Line
		DrawDebugLine(
			GetWorld(),
			MuzzleLocation,
			EndPoint,
			FColor::Red,
			true
		);
	}
}

