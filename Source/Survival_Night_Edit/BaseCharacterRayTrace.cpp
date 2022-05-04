// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterRayTrace.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h" 
#include "Kismet/GameplayStatics.h" 
#include "GameFramework/PlayerController.h" 

// Sets default values for this component's properties
UBaseCharacterRayTrace::UBaseCharacterRayTrace()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBaseCharacterRayTrace::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBaseCharacterRayTrace::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

// Called on shoot
void UBaseCharacterRayTrace::PlayerShootRayTrace(FVector &Location, FRotator &Rotation)
{
	UE_LOG(LogTemp, Warning, TEXT("Some warning message"));

	FVector StartPoint = Location + Rotation.Vector();
	FVector EndPoint = Location + Rotation.Vector() * 100;

	return;
}