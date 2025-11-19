// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"

#include "GameplayTagContainer.h"
#include "AbilitySystem/UB_AbilitySystemComponent.h"


#include "UB_Character.generated.h"

class UGameplayAbility;
class UBuster_StartUpData;
class UUB_AbilitySystemComponent;

UCLASS()
class ULTRABUSTER_API AUB_Character : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()


public:
	// Sets default values for this character's properties
	AUB_Character(const FObjectInitializer& ObjectInitializer);
	
	UAbilitySystemComponent* GetAbilitySystemComponent() const;
	
	virtual void PossessedBy(AController* NewController) override;
	void InitAbilityActorInfo();

	virtual void OnRep_PlayerState() override;

	
	UPROPERTY(BlueprintReadOnly)
	float NewPitchY;
	FRotator AimRotationY;
	// from to steal jump input
	bool bPressedUltraBusterJump;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void AbilityInputTagHeld(FGameplayTag InputTag);
	UPROPERTY(BlueprintReadOnly)
	bool bPressedSprint = false;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsAiming = false;
	
	void SprintPressed();
	void SprintReleased();
	void CrouchPressed();
	void CrouchReleased();

	void LedgeGrabPressed();

		

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LedgeGrabAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> UpwardMovementAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> LookAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr< UInputAction> SprintAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr< UInputAction> CrouchAction;


	void Look(const struct FInputActionValue& Value);

#pragma  region SST
	
	private:
	
	/** Custom CharacterMovementComponent */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTwoDCMC> TwoDCMC;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	/** Crouch/Drop Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> CrouchDropAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> MoveAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> DashAction;

	/** Time in seconds after dropping through a platform before the button becomes usable for jump input */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input, meta = (AllowPrivateAccess = "true"))
	float DropThroughPlatformJumpLockout = .2f;
	
	bool IsJumpStale = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void UpMoveInputPressed(const struct FInputActionValue& Value);

	//sst ref start
	/** Called for movement input */
	void Move(const struct FInputActionValue& Value);

	/** Called for crouch/drop input */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input")
	void CrouchDrop();

	/** Called when releasing crouch/drop input */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input")
	void StopCrouchDrop();

	/** Called when jump pressed, which could also be a drop-down command */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input")
	void JumpOrDrop();

	/** Called for dash input */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Input")
	void Dash();
	
public:
	/** Can override in blueprint for custom dash checking on this character */
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "Movement")
	bool CanDash() const;

	/** Called when releasing the jump button */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Movement")
	void ReleaseJump();

	/* Overrides to work with custom movement modes */
	virtual bool CanCrouch() const override; 

	virtual void ResetJumpState() override;

	virtual void Landed(const FHitResult& Hit) override;

	virtual void CheckJumpInput(float DeltaTime) override;

	/** Helper -- get collision query params to ignore this actor and its children*/
	virtual FCollisionQueryParams GetIgnoreSelfParams() const;

#pragma endregion

public:
// Effect Class and initialization functions for effects
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<class UGameplayEffect> DefaultPrimaryAttributes;
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;
	void InitializeDefaultAttributes() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FORCEINLINE UTwoDCMC* GetTwoDCMC() const { return TwoDCMC; }

	virtual void StopJumping() override;
	void Jump() override;

	bool bWantsToGrabLedge;

	void GrabLedge();

protected:
	//not from the data asset
	void GiveStartupAbilities();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability System", meta = (AllowPrivateAccess = "true"))
	 //class UUB_AbilitySystemComponent* AbilitySystemComponent;

	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	// Store Attribute Set Per Player
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	TSoftObjectPtr<UBuster_StartUpData> CharacterStartUpData;
private:

	UPROPERTY(EditDefaultsOnly, Category = "Buster|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	void Input_AbilityInputPressed(FGameplayTag InInputTag) ;
	void Input_AbilityInputReleased(FGameplayTag InInputTag) ;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	class UUB_InputConfig* InputConfigDataAsset;

	
	
};
