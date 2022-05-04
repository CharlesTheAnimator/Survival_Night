// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseShooterCharacter.generated.h"

class AGunBase;
class UBaseCharacterRayTrace;

UCLASS()
class SURVIVAL_NIGHT_EDIT_API ABaseShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseShooterCharacter();

	// Property replication 
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Gun muzzle offset from the camera location.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsShooting;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	//////////////////////////////////////////////////////////////////////////
		/*Camera Set Up*/
			
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)		//Spring Arm Component to follow the camera behind the player
	class USpringArmComponent* SpringArmComp;
			
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)		//Player follow camera
	class UCameraComponent* CameraComp;

	//////////////////////////////////////////////////////////////////////////
		/*Movement*/

	void MoveForward(float InputAxis);	// Called for forwards/backward input
	void MoveRight(float InputAxis);	// Called for left/right side input
	///void Turn(float InputAxis);		// Called when turning left or right-Old*/
	//void OnStartJump();				// Called for Jump start			-Old*/
	//void OnStopJump();				// Called for Jump end				-Old*/
	void BeginSprint();					// Sets Character Movement Speed to Sprint values.
	void EndSprint();					// Sets Character Movement Speed back to default speed values.
	void BeginCrouch();					// Request for Character to Crouch
	void EndCrouch();					// Request for Character to EndCrouch
	
	//UPROPERTY()						/*OLD
	//bool bIsMovingForward;			// Tracks direction of pawn for sprint logic
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsSprinting;					// Used for tracing sprint state
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bIsCrouching;					// Used for tracing crouch state

	//////////////////////////////////////////////////////////////////////////
		/*Weapons*/

	UFUNCTION()
	void StartFireGun();						// Called when player presses left mouse
	UFUNCTION()
	void EndFireGun();							// Called when player released left mouse or if !bFullAuto
	// TODO make FullAuto bool

	//////////////////////////////////////////////////////////////////////////
		/*Player Properties*/

		/*Properties to be Rep*/
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
	float Health;			// Holds Health Property

		/*RepNotify*/
	UFUNCTION()// For changes made to current health.
	void OnRep_CurrentHealth();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


private:
	/* UPROPERTY is a Macro that exposes this variable to Unreal’s reflection system, this
	is how the editor knows how to deal with/visualize this value and to show or hide it as
	a tweak-able value.  */


	//TODO make these client server reliable
	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
	float ImmuneSystem = 1.0;
	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
	float Stamina;
	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
	float Hunger;
	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
	float Thirst;
	UPROPERTY(EditDefaultsOnly, Category = "PlayerCondition")
	float Radiation;
	//Player Conditions Initial States, can be accessed in Blue print
	UPROPERTY(EditAnywhere, Category = "PlayerCondition")
	float MaxHealth = 100;
	UPROPERTY(EditAnywhere, Category = "PlayerCondition")
	float MaxStamina = 100;
	UPROPERTY(EditAnywhere, Category = "PlayerCondition")
	float MaxHunger = 10;
	UPROPERTY(EditAnywhere, Category = "PlayerCondition")
	float MaxThirst = 10;
	UPROPERTY(EditAnywhere, Category = "PlayerCondition")
	float MaxRadiation = 100;
	/* UFUNCTION follows the same theory, but is specialized to functions.
	In this case we expose the function to Blueprint and place it under “PlayerCondition” context
	menus of the editor. (you can specify any new category you wish. The “const” specifier
	specifies that no value inside this function is changed, it effectively turns this into into read only. */
	UFUNCTION(BlueprintCallable, Category = "PlayerCondition")
	float GetMaxHealth() const;

	//////////////////////////////////////////////////////////////////////////
		/*Player Gun Properties*/

	// Called to spawn Characters weapon with the GunBase class
	UPROPERTY(EditDefaultsOnly)		// Restricts Selection to children of GunBase class
	TSubclassOf<AGunBase> GunClass;
	UPROPERTY()						// Stores Chosen gun class
	AGunBase* Gun;
	UPROPERTY()
	UBaseCharacterRayTrace* RayTrace;
};