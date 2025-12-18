// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/UB_Character.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Data/Buster_StartUpData.h"
#include "AbilitySystemComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/UB_AbilitySystemComponent.h"
#include "ActorComponents/InventoryComponent.h"
#include "Characters/Player/UB_PlayerController.h"
#include "Components/ArrowComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Movement/TwoDCMC.h"
#include "Input/BusterInputComponent.h"
#include "Net/UnrealNetwork.h"
#include "Characters/Player/UB_PlayerState.h"
#include "Components/WidgetComponent.h"
#include "UI/HUD/BusterHUD.h"


AUB_Character::AUB_Character(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<UTwoDCMC>(ACharacter::CharacterMovementComponentName))
{
	// Re-enable ticking (REQUIRED because BaseCharacter turned it off)
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
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



void AUB_Character::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();
	GiveStartupAbilities();
	InitHealthBarWidget();
	InitAmmoWidget();
	BindCallbacksToDependencies();
	BroadcastInitialValues();
}

void AUB_Character::InitAbilityActorInfo()
{
	

	AUB_PlayerState* UB_PlayerState = GetPlayerState<AUB_PlayerState>();
	check(UB_PlayerState);
	UB_PlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(UB_PlayerState, this);
	Cast<UUB_AbilitySystemComponent>(UB_PlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();

	AbilitySystemComponent = UB_PlayerState->GetAbilitySystemComponent();

	AttributeSet = UB_PlayerState->GetAttributeSet();
	
	
	if (HasAuthority())
	{
		InitializeDefaultAttributes();
	}
	
	
	/*if (AUB_PlayerController* BusterPlayerController = Cast<AUB_PlayerController>(GetController()))
	{
		if (ABusterHUD* BusterHUD = Cast<ABusterHUD>(BusterPlayerController->GetHUD()))
		{
			BusterHUD->InitOverlay(BusterPlayerController, UB_PlayerState, AbilitySystemComponent, AttributeSet);
		}
	}*/
	
	
}

void AUB_Character::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();

	InitHealthBarWidget();
	InitAmmoWidget();
	BindCallbacksToDependencies();
	BroadcastInitialValues();
}

void AUB_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//corrections not made on the owning client 
	DOREPLIFETIME_CONDITION(AUB_Character, NewPitchY, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(AUB_Character, bIsAiming, COND_SkipOwner);
}

// Called when the game starts or when spawned
void AUB_Character::BeginPlay()
{
	Super::BeginPlay();
	
	// Inform a movement component of our initial orientation
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
	/*if (GEngine)
	{
		const FString ValueString= Value.ToString();
		const FString LogMsg = FString::Printf(TEXT("[InputBinding] Moving: %s"), *ValueString);
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, LogMsg);
	}*/
		
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

		//Stop Character From Performing Jump Before Executing Mantle Logic
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

}

void AUB_Character::Input_AbilityInputPressed(FGameplayTag InInputTag) 
{

		GetBusterAbilitySystemComponent()->AbilityInputTagPressed(InInputTag);
}

void AUB_Character::Input_AbilityInputReleased(FGameplayTag InInputTag) 
{
	GetBusterAbilitySystemComponent()->AbilityInputTagReleased(InInputTag);
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
// Shift to base
/*// helper function allows us to call multipler attribute sets on a character
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
}*/
// Called every frame
void AUB_Character::Tick(float DeltaTime)
{
	// Re-enable tick because BaseCharacter turned it off
	PrimaryActorTick.bCanEverTick = true; 
	PrimaryActorTick.bStartWithTickEnabled = true;
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


		BusterInputComponent->BindAction(EquipNextInputAction, ETriggerEvent::Triggered, this, &AUB_Character::OnEquipNextTriggered);


		BusterInputComponent->BindAction(DropItemInputAction, ETriggerEvent::Triggered, this, &AUB_Character::OnDropItemTriggered);
	

	
		BusterInputComponent->BindAction(UnequipInputAction, ETriggerEvent::Triggered, this, &AUB_Character::OnUnequipTriggered);




}

void AUB_Character::AbilityInputTagHeld(FGameplayTag InputTag)
{
	GetBusterAbilitySystemComponent()->AbilityInputTagHeld(InputTag);
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
//	FVector AimOrigin = GetActorLocation();

	// Get the Aim Origin from the spine socket
	FVector AimOrigin;
	if (GetMesh() && GetMesh()->DoesSocketExist(AimSocketName))
	{
		AimOrigin = GetMesh()->GetSocketLocation(AimSocketName);
	}
	else
	{
		// if I have not defined the socket
		AimOrigin = GetActorLocation() + FVector(0.f, 0.f, 60.f);
	}

	
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

// These probably need to go
void AUB_Character::OnDropItemTriggered(const FInputActionValue& Value)
{
	FGameplayEventData EventPayload;
	EventPayload.EventTag = UInventoryComponent::DropItemTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UInventoryComponent::DropItemTag, EventPayload);
}

void AUB_Character::OnEquipNextTriggered(const FInputActionValue& Value)
{
	FGameplayEventData EventPayload;
	EventPayload.EventTag = UInventoryComponent::EquipNextTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UInventoryComponent::EquipNextTag, EventPayload);
}

void AUB_Character::OnUnequipTriggered(const FInputActionValue& Value)
{
	FGameplayEventData EventPayload;
	EventPayload.EventTag = UInventoryComponent::UnequipTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, UInventoryComponent::UnequipTag, EventPayload);
}