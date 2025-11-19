// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/UB_Character.h"
#include "AbilitySystem/Data/Buster_StartUpData.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/UB_AbilitySystemComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Movement/TwoDCMC.h"
#include "Input/BusterInputComponent.h"
#include "Characters/Player/UB_PlayerState.h"




AUB_Character::AUB_Character(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<UTwoDCMC>(ACharacter::CharacterMovementComponentName))
{
	TwoDCMC = Cast<UTwoDCMC>(GetCharacterMovement());
	TwoDCMC->SetIsReplicated(true);

	//save for later
	// Create a configurable camera to follow the player
	/*
	FollowCamera = CreateDefaultSubobject<UFollowCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(RootComponent);
	FollowCamera->bUsePawnControlRotation = false;
	*/

	// Disable controller rotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetArrowComponent()->bHiddenInGame = true;

	JumpMaxCount = 2;

	// Tick and refresh bone transforms whether rendered or not - for bone updates on a dedicated server
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	//AbilitySystemComponent = CreateDefaultSubobject<UUB_AbilitySystemComponent>(TEXT("PlayerAbilitySystemComponent"));

	
}

UAbilitySystemComponent* AUB_Character::GetAbilitySystemComponent() const
{
	

	return AbilitySystemComponent;

}

void AUB_Character::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();
	GiveStartupAbilities();
}

void AUB_Character::InitAbilityActorInfo()
{
	

	AUB_PlayerState* UB_PlayerState = GetPlayerState<AUB_PlayerState>();
	check(UB_PlayerState);
	UB_PlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(UB_PlayerState, this);
	Cast<UUB_AbilitySystemComponent>(UB_PlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();

	AbilitySystemComponent = UB_PlayerState->GetAbilitySystemComponent();

	AttributeSet = UB_PlayerState->GetAttributeSet();
	

	InitializeDefaultAttributes();
	
	InitializeDefaultAttributes();
	
}

void AUB_Character::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();
}

// Called when the game starts or when spawned
void AUB_Character::BeginPlay()
{
	Super::BeginPlay();

	//Add Input Mapping Context
	/*
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	*/

	// Inform movement component of our initial orientation
	TwoDCMC->SetFacingRight(GetActorForwardVector().X > 0);
	//AbilitySystemComponent = Cast<UUB_AbilitySystemComponent>(GetAbilitySystemComponent());
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetComponentTickEnabled(true);
	}

	
}
void AUB_Character::UpMoveInputPressed(const struct FInputActionValue& Value)
{
	if (TwoDCMC) TwoDCMC->UpMoveInputPressed();
}


void AUB_Character::Move(const struct FInputActionValue& Value)
{

		float MovementValue = Value.Get<float>();
		TwoDCMC->AddInputVector(FVector::ForwardVector * MovementValue);
	
}

void AUB_Character::CrouchDrop_Implementation()
{
}

void AUB_Character::StopCrouchDrop_Implementation()
{
}

void AUB_Character::JumpOrDrop_Implementation()
{
}

void AUB_Character::Dash_Implementation()
{
}

bool AUB_Character::CanDash_Implementation() const
{
	return true;
}

void AUB_Character::ReleaseJump_Implementation()
{
}

bool AUB_Character::CanCrouch() const
{
	return Super::CanCrouch();
}


void AUB_Character::Jump()
{
	// 1) If we’re sliding, exit slide immediately
	if (TwoDCMC->IsCustomMovementMode(UBMOVE_Slide))
	{
		TwoDCMC->ExitSlide();

		//For Testing only [[PLEASE REMOVE FOR BETTER IMP]]
		TwoDCMC->Velocity.Z = FMath::Max<FVector::FReal>(TwoDCMC->Velocity.Z, TwoDCMC->JumpZVelocity);
		TwoDCMC->SetMovementMode(MOVE_Falling);
		
	}

	// 2) If we’re still crouched, uncrouch on the Character (this flips bWantsToCrouch, bIsCrouched, etc.)
	else if (TwoDCMC->IsCrouching())
	{
		UnCrouch();  
		// this will under-the-hood set bWantsToCrouch = false,
		// Safe_bPrevWantsToCrouch = false, and replicate properly
		
		//For Testing only [[PLEASE REMOVE FOR BETTER IMP]]
		TwoDCMC->Velocity.Z = FMath::Max<FVector::FReal>(TwoDCMC->Velocity.Z, TwoDCMC->JumpZVelocity);
		TwoDCMC->SetMovementMode(MOVE_Falling);
	}
	else
	{
		// 3) Now that we’re standing, call the normal jump
		Super::Jump();
		bPressedUltraBusterJump = true;

		//Stop Characther From Performing Jump Before Executing Mantle Logic
		//bPressedJump = false;
	}

	
	//bPressedUltraBusterJump = true;

	//Stop Character From Performing Jump Before Executing Mantle Logic
	// temp remove
	//bPressedJump = false;
}

void AUB_Character::GrabLedge()
{
	if (TwoDCMC) TwoDCMC->PressedGrabLedge();
}

void AUB_Character::GiveStartupAbilities()
{
	//if (!IsValid(GetAbilitySystemComponent())) return;

	UUB_AbilitySystemComponent* UB_ASC = CastChecked<UUB_AbilitySystemComponent>(AbilitySystemComponent);
	
	for (const auto& Ability : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability);
		/*if (GEngine && Ability)
		{
			const FString AbilityName = Ability->GetName();
			const FString LogMsg = FString::Printf(TEXT("[AbilityGrant] Startup Ability (No Tag): %s"), *AbilityName);
           
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, LogMsg);
		}*/
		UB_ASC->GiveAbility(AbilitySpec);
	}
	

	if (!CharacterStartUpData.IsNull())
	{
		if (UBuster_StartUpData* LoadedData = CharacterStartUpData.LoadSynchronous())
		{
			//LoadedData->GiveToAbilitySystemComponent(static_cast<UUB_AbilitySystemComponent*>(AbilitySystemComponent));
			LoadedData->GiveToAbilitySystemComponent(static_cast<UUB_AbilitySystemComponent*>(AbilitySystemComponent));			
		}
	}
	else
	{
		/*if (GEngine)
		{
			const FString LogMsg = FString::Printf(TEXT("Null"));
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Cyan, LogMsg);
		}*/
	}
}

void AUB_Character::Input_AbilityInputPressed(FGameplayTag InInputTag) 
{
	/*
	if (GEngine)
	{
		const FString LogMsg = FString::Printf(TEXT("AbilityInputTagPressed with Tag: %s"), *InInputTag.ToString());
		// Using -1 for the key, 5 seconds, in Red
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, LogMsg);
	}
	*/
	

	if (AbilitySystemComponent)
	{
	//	AbilitySystemComponent->AbilityInputTagPressed(InInputTag);
	}
}

void AUB_Character::Input_AbilityInputReleased(FGameplayTag InInputTag) 
{
	//AbilitySystemComponent->AbilityInputTagReleased(InInputTag);
}




void AUB_Character::StopJumping()
{
	IsJumpStale = false;
	Super::StopJumping();
	bPressedUltraBusterJump = false;
}

void AUB_Character::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	JumpCurrentCount = 0;
	JumpCurrentCountPreJump = 0;
}

void AUB_Character::CheckJumpInput(float DeltaTime)
{
	// In the default Character class, holding jump beyond JumpMaxHoldTime 
	//   will automatically trigger another jump. We override this method to 
	//   achieve the more expected behavior: the player must release the button
	//   and press it again to trigger an additional jump. 

	JumpCurrentCountPreJump = JumpCurrentCount;

	if (TwoDCMC)
	{
		if (bPressedJump && (IsJumpProvidingForce() || !IsJumpStale))
		{
			IsJumpStale = true;

			if (GetWorld()->GetTimeSeconds() - TwoDCMC->GetLastDropThroughPlatformTime() > DropThroughPlatformJumpLockout)
			{
				// If this is the first jump and we're already falling,
				// then increment the JumpCount to compensate.
				const bool bFirstJump = JumpCurrentCount == 0;
				if (bFirstJump && TwoDCMC->IsFalling())
				{
					JumpCurrentCount++;
				}

				const bool bDidJump = CanJump() && TwoDCMC->DoJump(bClientUpdating);
				if (bDidJump)
				{
					// Transition from not (actively) jumping to jumping.
					if (!bWasJumping)
					{
						JumpCurrentCount++;
						JumpForceTimeRemaining = GetJumpMaxHoldTime();
						OnJumped();
					}
				}

				bWasJumping = bDidJump;
			}
		}
	}
}

void AUB_Character::ResetJumpState()
{
	// Implements a fix to the default Character class, as its behavior is to
	//   reset jump count if this method is called in any state other than "Falling"
	// When extending custom CharacterMovementComponent, we add other states
	//   that shouldn't reset jump (wallslide, walljump, dash...)
	// Reset count in Landed() instead. 
	bPressedJump = false;
	bWasJumping = false;
	JumpKeyHoldTime = 0.0f;
	JumpForceTimeRemaining = 0.0f;
}


FCollisionQueryParams AUB_Character::GetIgnoreSelfParams() const
{
	FCollisionQueryParams Params;
	TArray<AActor*> SelfChildren;

	GetAllChildActors(SelfChildren);
	Params.AddIgnoredActors(SelfChildren);
	Params.AddIgnoredActor(this);

	return Params;
}

// helper function allows us to call multipler attribute sets on a character
void AUB_Character::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

// Called in the init ability actor function so that attributes are assigned after the ability system compnenet
void AUB_Character::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
}
// Called every frame
void AUB_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AUB_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	/*
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
	*/

		checkf(InputConfigDataAsset,TEXT("Forgot to assign a valid data asset as input config"));

		ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer();

		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

		check(Subsystem);

		Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext,0);

		UBusterInputComponent* BusterInputComponent = CastChecked<UBusterInputComponent>(PlayerInputComponent);
		
		// Jumping
		BusterInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AUB_Character::Jump);
		BusterInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AUB_Character::StopJumping);

		// Moving
		BusterInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AUB_Character::Move);
		
		// Looking
		BusterInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AUB_Character::Look);

		// Sprinting
		BusterInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AUB_Character::SprintPressed);
		BusterInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AUB_Character::SprintReleased);

		// Crouching
		BusterInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AUB_Character::CrouchPressed);
		BusterInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AUB_Character::CrouchReleased);

		BusterInputComponent->BindAction(LedgeGrabAction, ETriggerEvent::Completed, this, &AUB_Character::GrabLedge);
		
		//LedgeGrabPressed
		//Mantle
		BusterInputComponent->BindAction(UpwardMovementAction, ETriggerEvent::Completed, this, &AUB_Character::UpMoveInputPressed);
		
		BusterInputComponent->BindAbilityActions(InputConfigDataAsset,this,&ThisClass::Input_AbilityInputPressed,&ThisClass::Input_AbilityInputReleased, &ThisClass::AbilityInputTagHeld);


		
	/*}
	else
	{
		//UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}*/

}

void AUB_Character::AbilityInputTagHeld(FGameplayTag InputTag)
{
	//AbilitySystemComponent->AbilityInputTagHeld(InputTag);
}
void AUB_Character::SprintPressed()
{
	bPressedSprint = true;
	if (TwoDCMC) TwoDCMC->SprintPressed();

}

void AUB_Character::SprintReleased()
{
	bPressedSprint = false;
	if (TwoDCMC) TwoDCMC->SprintReleased();

}

void AUB_Character::CrouchPressed()
{
	//if (TwoDCMC && TwoDCMC->IsHanging()) TwoDCMC->EnterWallslide();
	if (TwoDCMC) TwoDCMC->CrouchPressed();

}

void AUB_Character::CrouchReleased()
{
	if (TwoDCMC) TwoDCMC->CrouchReleased();

}

void AUB_Character::LedgeGrabPressed()
{
	if (TwoDCMC && !TwoDCMC->IsHanging()) bWantsToGrabLedge = true;
}

void AUB_Character::Look(const struct FInputActionValue& Value)
{
	const FVector2D LookInput = Value.Get<FVector2D>();

	const float InX = LookInput.X;
	const float InY = LookInput.Y;

	// Pass horizontal aim direction to movement component (handles flipping)
	/*TwoDCMC->RequestFaceDirection(InX);*/

	if (UTwoDCMC* CMC = Cast<UTwoDCMC>(GetCharacterMovement()))
	{
		if (CMC->Safe_bWantsToSprint)
		{
			bIsAiming = false;
			NewPitchY = 0;
			return;
			
		}
		CMC->SetAimInput(LookInput, 0.10f);
		CMC->ApplyAimFacing(); 
	}

	// Convert aim to world-space rotator for animation/weapon alignment
	FVector AimOrigin = GetActorLocation();
	const FVector StickInput = FVector(InX, 0.f, InY) * 100.f;

	FVector StickLocation = AimOrigin + StickInput;

	// Direction from origin to stick location
	FVector AimVec = StickLocation - AimOrigin;

	// Convert aim vector to rotation
	FRotator AimRotation = UKismetMathLibrary::MakeRotFromX(AimVec);

	// Only map when input magnitude is significant
	if (LookInput.SizeSquared() > .1f)
	{
		NewPitchY = FMath::GetMappedRangeValueClamped(
			FVector2D(-90.f, 90.f),
			FVector2D(-1.f, 1.f),
			AimRotation.Pitch
		);

		bIsAiming = true;
	}
	else
	{
		bIsAiming = false;
	}

	UE_LOG(LogTemp, Verbose, TEXT("Pitch: %f  ->  NewPitchY: %f"), AimRotation.Pitch, NewPitchY);
	
	/*if (!FMath::IsNearlyZero(InX) || !FMath::IsNearlyZero(InY))
	{
		NewPitchY = UKismetMathLibrary::MapRangeClamped(AimRotation.Pitch,-90.f,90.f, -1.f, 1.f);
	}*/
	
	

}
