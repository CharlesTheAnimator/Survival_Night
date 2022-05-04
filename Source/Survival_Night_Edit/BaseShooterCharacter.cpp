// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h" 

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h" 

#include "Components/InputComponent.h"
#include "DrawDebugHelpers.h"
#include "GunBase.h"
#include "BaseCharacterRayTrace.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

//////////////////////////////////////////////////////////////////////////
/*Constructor*/
// Sets default values
ABaseShooterCharacter::ABaseShooterCharacter()
{ /*You can turn this off to improve performance if you don't need it.*/
 	// Set this character to call Tick() every frame. 
	PrimaryActorTick.bCanEverTick = true;

	// Instantiating your class Components
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));

	// Set the location and rotation of the Character Mesh Transform
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FQuat(FRotator(0.0f, -90.0f, 0.0f)));

	// Attaching your class Components to the default character's Skeletal Mesh Component.
	SpringArmComp->SetupAttachment(GetMesh());
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);

	//Setting class variables of the spring arm
	SpringArmComp->bUsePawnControlRotation = true;

	// Setting class variables of the Character movement component
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bIgnoreBaseRotation = true;
}

//////////////////////////////////////////////////////////////////////////
/*Gameplay Functions*/
// Called when the game starts or when spawned
void ABaseShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Set Character Starting Conditions
	Health = MaxHealth;
	Stamina = MaxStamina;
	Thirst = MaxThirst;
	Hunger = MaxHunger;
	Radiation = MaxRadiation;

	// Set Gun Starting Conditions

	/*NOTE: By setting the gun class pointer defined in the header as the spawned gun actor, one line of code can both spawn 
	the selected weapon and set it to the class pointer. AttachToComponent is inherited from actor.h It takes in 3 arguments
	1. the mesh to be attached to component (here we are calling this function through the class pointer to GunBase thus 
	letting GetMesh() be a valid target to get the gun mesh) 2. A transform rule (These are pre-determined rules that can
	be tracked down through Actor.h) 3. The socket that the new mesh should attach to. Finally we set this class to own the gun*/

	Gun = GetWorld()->SpawnActor<AGunBase>(GunClass);		// Looks through GunClass to spawn proper gun
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("hand_rWeaponSocket"));
	Gun->SetOwner(this);									// Sets Gun to being owned by this class
}
// Called every frame
void ABaseShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
	/*Blueprint UFuntions*/
// Called to Send Max Health Value to Blueprint
float ABaseShooterCharacter::GetMaxHealth() const
{
	return MaxHealth;
}

//////////////////////////////////////////////////////////////////////////
/*Player Input & Movement Control Mapping*/
	/*Key Bindings*/
// Called to bind functionality to input
void ABaseShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Movement 
	PlayerInputComponent->BindAxis("MoveForward", this, &ABaseShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABaseShooterCharacter::MoveRight);

	/*Old - Handled in BP*/
	// Looking up/down/sideways is already supported in APawn.h, so we simply reference the existing functions.
	//PlayerInputComponent->BindAxis("Turn", this, &ABaseShooterCharacter::Turn);
	//PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

		/* There is an overload (meaning a variation with a different set of parameters,
		but equal function name) available to specify when you wish the function to execute. 
		This parameter is only available for BindAction function and not the above BindAxis. */

	// Jump binding to space bar
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Released, this, &ACharacter::StopJumping);
	
	// Sprint binding to left shift
	PlayerInputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &ABaseShooterCharacter::BeginSprint);
	PlayerInputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &ABaseShooterCharacter::EndSprint);

	// Crouch binding to left ctrl
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &ABaseShooterCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Released, this, &ABaseShooterCharacter::EndCrouch);

	// Firing weapon to left mouse
	PlayerInputComponent->BindAction("Shoot", EInputEvent::IE_Pressed, this, &ABaseShooterCharacter::StartFireGun);
	PlayerInputComponent->BindAction("Shoot", EInputEvent::IE_Released, this, &ABaseShooterCharacter::EndFireGun);

}

	/*Locomotion Logic | X & Y plane*/
//Called every frame while 'w || s' is pressed: X
void ABaseShooterCharacter::MoveForward(float AxisValue)
{
	if ((Controller != nullptr) && (AxisValue != 0.0f))
	{
		// Find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, AxisValue);

		// Check for sprint
		/*Alternate sprint handling*/
		//if((bIsSprinting) && (AxisValue >= 0.5f))
		//{AddMovementInput(Direction, AxisValue, true);}
	}
	// Added logic for fine control of sprint. Players should only sprint in forward direction
	//AxisValue > (0.0f) ? bIsMovingForward = true : bIsMovingForward = false;
}
//Called every frame while 'a || d' is pressed: Y
void ABaseShooterCharacter::MoveRight(float AxisValue)
{
	if ((Controller != nullptr) && (AxisValue != 0.0f))
	{
		// Find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Add movement in that direction
		AddMovementInput(Direction, AxisValue);

		/*Old*/
		// Added logic for tick driven sprint
		//bIsMovingForward = false;
	}
}


/*Jump Logic - old*/////////////////////////////////////////////////////////////////////////////

	/*Un-Comment to compile jump in this level. Otherwise handled in ACharacter class.
	Doing this requires changing the binding logic and Un-commenting logic in header.
	look for JumpStart & JumpStop under movement section*/

//Called when space bar pressed
//void ABaseShooterCharacter::OnStartJump()
//{
//	Jump();
//}
// Called when space bar is released
//void ABaseShooterCharacter::OnStopJump()
//{
//	StopJumping();
//}////////////////////////////////////////////////////////////////////////////////////////////

	/*Crouch Logic*/
//Called when left ctrl is pressed
void ABaseShooterCharacter::BeginCrouch()
{
	Crouch();
	bIsCrouching = true;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
}
// Called when left ctrl is released
void ABaseShooterCharacter::EndCrouch()
{
	UnCrouch();
	bIsCrouching = false;
	bIsSprinting ? GetCharacterMovement()->MaxWalkSpeed = 1000.0f : GetCharacterMovement()->MaxWalkSpeed = 600.0f;

}

	/*Sprint Logic*/
// Called when left shift is pressed
void ABaseShooterCharacter::BeginSprint()
{
	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = 1000.0f;
}
// Called when left shift is released
void ABaseShooterCharacter::EndSprint()
{
	bIsSprinting = false;
	bIsCrouching ? GetCharacterMovement()->MaxWalkSpeed = 300.0f : GetCharacterMovement()->MaxWalkSpeed = 600.0f;

}


/*Turning logic*///////////////////////////////////////////////////////////
/*Old - Logic Moved to BP*/
// Called when turning left or right
//void ABaseShooterCharacter::Turn(float AxisValue) {
//	AddControllerYawInput(AxisValue);
/* 

		Turning is now handled in BP 

*/
///////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
// Weapon & Damage Logic

void ABaseShooterCharacter::StartFireGun()
{
	if (Gun != nullptr)
	{
		Gun->FireGun(true);
		bIsShooting = true;
	}

}

void ABaseShooterCharacter::EndFireGun()
{
	bIsShooting = false;
}


//////////////////////////////////////////////////////////////////////////
// Replicated Properties

void ABaseShooterCharacter::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate current health.
	DOREPLIFETIME(ABaseShooterCharacter, Health);
}

void ABaseShooterCharacter::OnRep_CurrentHealth()
{

}