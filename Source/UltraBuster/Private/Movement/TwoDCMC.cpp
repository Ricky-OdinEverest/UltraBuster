// Fill out your copyright notice in the Description page of Project Settings.


#include "Movement/TwoDCMC.h"
#include "Characters/UB_Character.h"
#include "Components/CapsuleComponent.h"
#include "DSP/AudioDebuggingUtilities.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"

#if 1
float MacroDuration = 1.f;
// __LINE__ should make a line based key
#define SLOG(x) GEngine->AddOnScreenDebugMessage(__LINE__, MacroDuration ? MacroDuration : -1.f, FColor::Yellow, x);
#define POINT(x, c) DrawDebugPoint(GetWorld(), x, 10, c, !MacroDuration, MacroDuration);
#define LINE(x1, x2, c) DrawDebugLine(GetWorld(), x1, x2, c, !MacroDuration, MacroDuration);
#define CAPSULE(x, c) DrawDebugCapsule(GetWorld(), x, CapHH(), CapR(), FQuat::Identity, c, !MacroDuration, MacroDuration);
#else
#define SLOG(x)
#define POINT(x, c)
#define LINE(x1, x2, c)
#define CAPSULE(x, c)
#endif



#pragma  region SavedMove_and_PredictionData

// init struct variables

//changed facing right to 1
UTwoDCMC::FSavedMove_UltraBuster::FSavedMove_UltraBuster(): Saved_bWantsToSprint(0), Saved_bWantsToDash(0),
                                                            Saved_bPressedUltraBusterJump(0), Saved_bPrevWantsToCrouch(0),
                                                            Saved_bWantsToProne(0),
                                                            Saved_bFacingRight(1),
                                                            Saved_bHadAnimRootMotion(0),Saved_bTransitionFinished(0)
{
	
}

/** Returns true if this move can be combined with NewMove for replication without changing any behavior */

bool UTwoDCMC::FSavedMove_UltraBuster::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter,
	float MaxDelta) const
{
	const FSavedMove_UltraBuster* NewUltraBusterMove = static_cast<FSavedMove_UltraBuster*>(NewMove.Get());

	if (Saved_bWantsToSprint != NewUltraBusterMove->Saved_bWantsToSprint) { return false; }

	if (Saved_bWantsToDash != NewUltraBusterMove->Saved_bWantsToDash) { return false; }

	// don't combine opposite-sided actions. (mainly wall related)
	if (Saved_bFacingRight != NewUltraBusterMove->Saved_bFacingRight) { return false; }

	if (Saved_bWantsToMantle != NewUltraBusterMove->Saved_bWantsToMantle) { return false; }

	return FSavedMove_Character::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UTwoDCMC::FSavedMove_UltraBuster::Clear()
{
	FSavedMove_Character::Clear();

	// Flags
	// are replicated, I believe
	Saved_bWantsToSprint = 0;
	Saved_bWantsToDash = 0;
	// flag used to attempt to run custom jump logic before default jump logic
	Saved_bPressedUltraBusterJump = 0;

	
	// Other Variables
	// Possible for more of these to cause desysnc since they are derived from other variables?
	Saved_bPrevWantsToCrouch = 0;
	Saved_bWantsToProne = 0;
	

	// flags dealing with animations like mantle dash and hang
	Saved_bHadAnimRootMotion = 0;
	Saved_bTransitionFinished = 0;


	// Default is true for facing right
	Saved_bFacingRight = 1;
}

//Returns a byte containing encoded special movement information (jumping, crouching, etc.)
// One way the client will replicate movement data
// Calling this flips the compressed flag value
// Used for movement that needs to be checked constantly to confirm its output but does not send a ton of data

uint8 UTwoDCMC::FSavedMove_UltraBuster::GetCompressedFlags() const
{
	uint8 Result = FSavedMove_Character::GetCompressedFlags();

	// In sst this syncs rotation so I might get rid of it to free up the flag and opt for instant rotation
	if (Saved_bFacingRight) Result |=  FLAG_Right;

	if (Saved_bWantsToSprint) Result |= FLAG_Sprint;
	if (Saved_bWantsToDash) Result |= FLAG_Dash;

	//mapping both jump and zippy jump to the jump pressed flag
	if (Saved_bPressedUltraBusterJump) Result |= FLAG_JumpPressed;

	if (Saved_bWantsToMantle) Result |= FLAG_Custom_3;
	
	return Result;
}
//Set value of server flags to value of client bool
void UTwoDCMC::FSavedMove_UltraBuster::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel,
	FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	UTwoDCMC* CharacterMovement;
	getAttachedBusterMovement(C, CharacterMovement);

	Saved_bWantsToSprint = CharacterMovement->Safe_bWantsToSprint;
	Saved_bPrevWantsToCrouch = CharacterMovement->Safe_bPrevWantsToCrouch;

	// Jump Bool is on the character Owner
	Saved_bPressedUltraBusterJump = CharacterMovement->UB_CharacterOwner->bPressedUltraBusterJump;
	
	Saved_bHadAnimRootMotion = CharacterMovement->Safe_bHadAnimRootMotion;
	Saved_bTransitionFinished = CharacterMovement->Safe_bTransitionFinished;

	
	Saved_bWantsToProne = CharacterMovement->Safe_bWantsToProne;
	Saved_bWantsToDash = CharacterMovement->Safe_bWantsToDash;

	Saved_bFacingRight = CharacterMovement->Safe_bFacingRight;

	Saved_bWantsToMantle = CharacterMovement->Safe_bWantsToMantle;
}



// Called before ClientUpdatePosition uses this SavedMove to make a predictive correction
// Or just applied the saved move value in setmove to the client from the server
void UTwoDCMC::FSavedMove_UltraBuster::PrepMoveFor(ACharacter* C)
{
	FSavedMove_Character::PrepMoveFor(C);

	UTwoDCMC* CharacterMovement;
	getAttachedBusterMovement(C, CharacterMovement);

	CharacterMovement->Safe_bWantsToSprint = Saved_bWantsToSprint;
	CharacterMovement->Safe_bPrevWantsToCrouch = Saved_bPrevWantsToCrouch;
	
	CharacterMovement->UB_CharacterOwner->bPressedUltraBusterJump = Saved_bPressedUltraBusterJump;
	
	
	CharacterMovement->Safe_bHadAnimRootMotion = Saved_bHadAnimRootMotion;
	CharacterMovement->Safe_bTransitionFinished = Saved_bTransitionFinished;
	
	CharacterMovement->Safe_bWantsToProne = Saved_bWantsToProne;
	CharacterMovement->Safe_bWantsToDash = Saved_bWantsToDash;

	CharacterMovement->Safe_bFacingRight = Saved_bFacingRight;

	CharacterMovement->Safe_bWantsToMantle = Saved_bWantsToMantle;
}
// Here we tell the CMC we are using our own custom saved move class
// which is only this : Super(ClientMovement)
UTwoDCMC::FNetworkPredictionData_Client_UltraBuster::FNetworkPredictionData_Client_UltraBuster(
	const UCharacterMovementComponent& ClientMovement)
: Super(ClientMovement)
{
}

FSavedMovePtr UTwoDCMC::FNetworkPredictionData_Client_UltraBuster::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_UltraBuster());
}
//The getter that lets us use the custom saved move class
FNetworkPredictionData_Client* UTwoDCMC::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		// we have to const cast because the cmc is non const
		UTwoDCMC* MutableThis = const_cast<UTwoDCMC*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_UltraBuster(*this);
		// alters how network corrections are handled
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f; 
	}
	return ClientPredictionData;
}

UTwoDCMC::UTwoDCMC()
{
	NavAgentProps.bCanCrouch = true;
}


#pragma endregion

#pragma region Component_Overrides

void UTwoDCMC::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	
	FString ModeString = GetMovementModeAsString(MovementMode, CustomMovementMode);
	SLOG(FString::Printf(TEXT("Movement Mode: %s"), *ModeString));


}


void UTwoDCMC::InitializeComponent()
{
	Super::InitializeComponent();

	UB_CharacterOwner = Cast<AUB_Character>(GetOwner());
}

bool UTwoDCMC::IsMovingOnGround() const
{
	// hopefully this allows us to shrink the capsule 
	return Super::IsMovingOnGround() || IsCustomMovementMode(UBMOVE_Slide);
//  
	
}

#pragma endregion

#pragma region CharacterMovementOverrides

//From server to client
void UTwoDCMC::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
	
	//local boolean filled with flag value
	Safe_bWantsToSprint = (Flags & FSavedMove_UltraBuster::FLAG_Sprint) != 0;
	
	Safe_bWantsToDash = (Flags & FSavedMove_UltraBuster::FLAG_Dash) != 0;

	Safe_bFacingRight = (Flags & FSavedMove_UltraBuster::FLAG_Right) != 0;

	Safe_bWantsToMantle = (Flags & FSavedMove_UltraBuster::FLAG_Custom_3) != 0;
}

void UTwoDCMC::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	
	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == UBMOVE_Slide) ExitSlide();
	//can make work atm
	//if (IsCustomMovementMode(UBMOVE_Slide)) EnterSlide(PreviousMovementMode, (EUBCharacterMovementMode)(PreviousCustomMode));
	
	if (!UB_CharacterOwner) { return; }
	if (UB_CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		if (IsDashing)
		{
			if (MovementMode != MOVE_Custom || CustomMovementMode != UBMOVE_DASHING)
			{
				IsDashing = false;
			}
		}

		if (IsWallsliding)
		{
			// Wallsliding is "buffered" when dashing into a wall, while waiting for dash to complete
			if (MovementMode != MOVE_Custom || (CustomMovementMode != UBMOVE_WALLSLIDING && CustomMovementMode != UBMOVE_DASHING))
			{
				IsWallsliding = false;
			}
		}
	}

	
	
}

void UTwoDCMC::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	
	// Slide
	if (MovementMode == MOVE_Walking
	&& bWantsToCrouch               // you just pressed crouch
	&& !Safe_bPrevWantsToCrouch     // you weren’t crouching last frame
	&& Safe_bWantsToSprint )         // you’re sprinting
	{
		if (CanSlide())
		{
			bWantsToCrouch = true;
			Crouch(false);
			//Velocity.X += Velocity.GetSafeNormal2D() * SlideEnterImpulse;

			FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, true, NULL);
			
			SetMovementMode(MOVE_Custom, UBMOVE_Slide);
			
		}
	}
	else if (IsCustomMovementMode(UBMOVE_Slide) && !bWantsToCrouch)
	{
		SetMovementMode(MOVE_Walking);
	}
	else if (IsFalling() && bWantsToCrouch)
	{
		//if (TryClimb())
		//bWantsToCrouch = false;
	}
	
	// Check if we can re-enable jumping after a walljump
	if (HasRecentlyWalljumped)
	{
		if (GetWorld()->GetTimeSeconds() > LastWallJumpTime + WallslideJumpInputDisableSeconds)
		{
			HasRecentlyWalljumped = false;
		}
	}

	// Check if the buffer to convert a jump into a walljump has passed
	if (HasRecentlyWallSlid)
	{
		if (GetWorld()->GetTimeSeconds() > LastWallslideFallingReleaseTime + WallslideReleaseWalljumpBufferSeconds)
		{
			HasRecentlyWallSlid = false;
		}
	}

	// Check for wallslide
	// If we are in the air and holding in our forward direction, removed negative z requirement
	if (IsFalling() && IsHeadedForwards() && !IsDashing)
	{
		// Check for a wall in front and no floor beneath
		FHitResult Floor;
		if (!GetValidFloorBeneath(Floor))
		{
			TArray<FHitResult> HitResultsWall;
			GetWallslideWall(HitResultsWall);
			for (FHitResult Hit : HitResultsWall)
			{
				if (IsEligibleWallForSliding(Hit))
				{
					EnterWallslide();
				}
			}
		}
	}
	
	// Try LedgeGrab commentMore actions
	//if (UB_CharacterOwner->bWantsToGrabLedge)
	if (Safe_bWantsToMantle)
	{
		if (TryLedgeGrab())
		{
				UB_CharacterOwner->StopJumping();
				Safe_bWantsToMantle = false;
		}

		else
		{
		
			//UB_CharacterOwner->bWantsToGrabLedge = false;
			//CharacterOwner->bPressedJump = true;
			//CharacterOwner->CheckJumpInput(DeltaSeconds);
		}
	}

	// Transition LedgeGrab
	if (Safe_bTransitionFinished)
	{
		if (TransitionName == "LedgeGrab")
		{
			
			
			SetMovementMode(MOVE_Custom, UBMOVE_Hang);
			UB_CharacterOwner->JumpCurrentCount = 0;
			Velocity = FVector::ZeroVector;
		}
		else if (TransitionName == "Mantle")
		{
			//CharacterOwner->PlayAnimMontage(TallLedgeGrabMontage, 1 / TransitionRMS->Duration);


			// setting this further down
			
			/*SetMovementMode(MOVE_Flying);
			CharacterOwner->PlayAnimMontage(TallLedgeGrabMontage, 1 / 1.f);
			TransitionQueuedMontageSpeed = 0.f;
			TransitionQueuedMontage = nullptr;*/
	
		}
		//this was for mantle
		/*else
		{
			SLOG("WalkSwitch")
			SetMovementMode(MOVE_Walking);
		}*/
		Safe_bTransitionFinished = false;
		TransitionName = "";
	}
	
	if(IsCustomMovementMode(UBMOVE_Hang) && !HasAnimRootMotion())
	{
		if(bWantsToCrouch)
		{
			//
			
			SetMovementMode(MOVE_Falling);
			//bWantsToCrouch = false;
		}
		else if (Safe_bWantsToMantle)
		{
			
			SetMovementMode(MOVE_Flying);
			Turning = true;
			CharacterOwner->PlayAnimMontage(MatleMontage, MantleSpeed);
			TransitionQueuedMontageSpeed = 0.f;
			TransitionQueuedMontage = nullptr;
			Safe_bWantsToMantle = false;
		}
		else
		{
			
		}
	}


	
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UTwoDCMC::UpdateCharacterStateAfterMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateAfterMovement(DeltaSeconds);
	
	//Walk after ledge Grab
	if (!HasAnimRootMotion() && Safe_bHadAnimRootMotion && IsMovementMode(MOVE_Flying))
	{
		
		SetMovementMode(MOVE_Walking);
	}
	// Set transision finished to true and
	if (GetRootMotionSourceByID(TransitionRMS_ID) && GetRootMotionSourceByID(TransitionRMS_ID)->Status.HasFlag(ERootMotionSourceStatusFlags::Finished))
	{
		// Manual Removal is likely not needed
		RemoveRootMotionSourceByID(TransitionRMS_ID);
		Safe_bTransitionFinished = true;
	}
	
	Safe_bHadAnimRootMotion = HasAnimRootMotion();
}

void UTwoDCMC::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
	if (IsMovementMode(MOVE_Flying) && !HasRootMotionSources())
	{
		
		SetMovementMode(MOVE_Walking);
	}
	Safe_bPrevWantsToCrouch = bWantsToCrouch;
}

void UTwoDCMC::PhysCustom(float DeltaTime, int32 Iterations)
{
	Super::PhysCustom(DeltaTime, Iterations);

	switch (CustomMovementMode)
	{
	case UBMOVE_Hang:
		//PhysHang(DeltaTime, Iterations);
		break;
	case UBMOVE_Slide:
		PhysSlide(DeltaTime, Iterations);
		break;
	case UBMOVE_WALLSLIDING:
		PhysWallslide(DeltaTime, Iterations);
		break;
	case UBMOVE_DASHING:
		PhysDash(DeltaTime, Iterations);
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("Unrecognized custom movement mode"))
	}
}

void UTwoDCMC::StartNewPhysics(float deltaTime, int32 Iterations)
{
//I will have to review this when it comes to the gun logic
	FVector forward = UB_CharacterOwner->GetActorForwardVector();
	if ((Safe_bFacingRight && (forward.X < 1.f - FLT_EPSILON))
		|| (!Safe_bFacingRight && (forward.X > -1.f + FLT_EPSILON)))
	{
		float angleFromXAxis = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(forward, (Safe_bFacingRight ? 1 : -1) * FVector::ForwardVector)));

		FRotator RotationToAdd(0, FMath::Clamp((Safe_bFacingRight ? -1 : 1) * deltaTime * 180 / TurnAroundSeconds, -angleFromXAxis, angleFromXAxis), 0);

		FHitResult Hit(1.0f);
		SafeMoveUpdatedComponent(FVector::ZeroVector, UpdatedComponent->GetComponentQuat() * RotationToAdd.Quaternion(), false, Hit);
	}
	else // finished turning
	{
		Turning = false;
	}

	Super::StartNewPhysics(deltaTime, Iterations);
}

float UTwoDCMC::GetMaxSpeed() const
{
	float MaxSprintSpeed = 800.f;
	//Sprinting Currently Entered Here 
	if (IsMovementMode(MOVE_Walking) && Safe_bWantsToSprint && !IsCrouching()) return MaxSprintSpeed;
	
	if (MovementMode != MOVE_Custom) { return Super::GetMaxSpeed(); }
	
	switch (CustomMovementMode)
	{
	case UBMOVE_Hang:
		return 0.f;
	case UBMOVE_Slide:
		return MaxSlideSpeed;
	case UBMOVE_WALLSLIDING:
		return WallslideSpeed;
	case UBMOVE_DASHING:
		return DashDistance / DashDurationSeconds;
	default:
		UE_LOG(LogTemp, Error, TEXT("Attempt to get max speed with an unknown movement mode"))
		return 0.0f;
	}
}

float UTwoDCMC::GetMaxBrakingDeceleration() const
{
	if (MovementMode != MOVE_Custom) { return Super::GetMaxBrakingDeceleration(); }

	switch (CustomMovementMode)
	{
	case UBMOVE_Hang:
		return 0.f;
	case UBMOVE_Slide:
		return BrakingDecelerationSliding;
	case UBMOVE_WALLSLIDING:
		return 0.0f;
	case UBMOVE_DASHING:
		return 0.0f;
	default:
		UE_LOG(LogTemp, Error, TEXT("Attempt to get max braking deceleration with an unknown movement mode"))
		return 0.0f;
	}
}

bool UTwoDCMC::CanAttemptJump() const
{
	// need to check vars here
	return Super::CanAttemptJump() || CanWalljump() || (CanJumpCancelDash && IsDashing) || IsHanging();
}

void UTwoDCMC::ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations)
{
	Super::ProcessLanded(Hit, remainingTime, Iterations);

	CurrentDashesBeforeLanding = 0;
}

FString UTwoDCMC::GetMovementModeAsString(EMovementMode MoveMode, uint8 CustomMode) const
{
	switch (MoveMode)
	{
	case MOVE_None:
		return TEXT("None");
	case MOVE_Walking:
		return TEXT("Walking");
	case MOVE_NavWalking:
		return TEXT("NavWalking");
	case MOVE_Falling:
		return TEXT("Falling");
	case MOVE_Swimming:
		return TEXT("Swimming");
	case MOVE_Flying:
		return TEXT("Flying");

	case MOVE_Custom:
		{
			switch (CustomMode)
			{
			case UBMOVE_WALLSLIDING: return TEXT("Wallsliding");
			case UBMOVE_WallRun:     return TEXT("WallRun");
			case UBMOVE_DASHING:     return TEXT("Dashing");
			case UBMOVE_Slide:       return TEXT("Slide");
			case UBMOVE_Prone:       return TEXT("Prone");
			case UBMOVE_Hang:        return TEXT("Hang");
			default:                 return FString::Printf(TEXT("Custom(%d)"), CustomMode);
			}
		}

	default:
		return TEXT("Unknown");
	}
}

void UTwoDCMC::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	
	DOREPLIFETIME_CONDITION(UTwoDCMC, Safe_bFacingRight, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(UTwoDCMC, IsWallsliding, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(UTwoDCMC, IsDashing, COND_SimulatedOnly);
	DOREPLIFETIME_CONDITION(UTwoDCMC, Proxy_bLedgeGrabbed, COND_SimulatedOnly);

	DOREPLIFETIME_CONDITION(UTwoDCMC, Safe_bWantsToSprint, COND_SimulatedOnly);
}


#pragma endregion

#pragma region SST

#pragma region SST_Wallslide_AND_Jump
void UTwoDCMC::EnterWallslide()
{
	if (UB_CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		IsWallsliding = true;
	}
	InitialWallSlideZ = Velocity.Z;
	SetMovementMode(MOVE_Custom, EUBCharacterMovementMode::UBMOVE_WALLSLIDING);
	LastWallWasRelativeRight = Safe_bFacingRight;
	OnBeginWallslide();
}

void UTwoDCMC::ExitWallslide()
{
	if (UB_CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		IsWallsliding = false;
	}
	// this may belong in update state after movement
	FHitResult Floor;
	if (GetValidFloorBeneath(Floor)) // land
	{
		if (CharacterOwner)
		{
			CharacterOwner->Landed(Floor);
		}
		CurrentDashesBeforeLanding = 0;
		SetMovementMode(MOVE_Walking);
	}
	else // fall
	{
		SetMovementMode(MOVE_Falling);
		HasRecentlyWallSlid = true;
		LastWallslideFallingReleaseTime = GetWorld()->GetTimeSeconds();
	}
	OnEndWallslide();
}

void UTwoDCMC::PhysWallslide(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME) { return; }

	// During wallslide we intentionally:
	// - Do not call CalcVelocity
	// - Disregard leftover root motion
	// 
	// End the wallslide if: we land, user input stops, we lose our ability to wallslide, or we are no longer against a wall
	// Simulated proxies should not manually end their own wallslide state
	if (UB_CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		bool FinishWallslide = false;
		TArray<FHitResult> HitResultsWall;
		FHitResult Floor;
		//removing to see if I stick to wall
	//!IsHeadedForwards()	||
	if (
			 !CanWallSlide
			|| GetValidFloorBeneath(Floor)
			|| !GetWallslideWall(HitResultsWall))
		{
			FinishWallslide = true;
		}

		bool SuitableWall = false;
		for (FHitResult Hit : HitResultsWall)
		{
			if (IsEligibleWallForSliding(Hit))
			{
				SuitableWall = true;
				break;
			}
		}
		if (FinishWallslide || !SuitableWall)
		{
			ExitWallslide();
			StartNewPhysics(DeltaTime, Iterations);
			return;
		}
	}
	
	float TargetZ = WallslideSpeed * FVector::DownVector.Z; // negative downward speed
	float TaperRate = 4.f; // tune decay speed

	// preserve upward momentum and taper it toward slide speed
	Velocity.Z = FMath::FInterpTo(InitialWallSlideZ, TargetZ, DeltaTime, TaperRate);
	InitialWallSlideZ = Velocity.Z;

	/*// true if up velocity greater than 0
	bool bVelUp = Velocity.Z > 0.f;
	// No acceleration during a wallslide
	Velocity = WallslideSpeed * FVector::DownVector;

	// Gravity scales with how much the player is pulling away from the acceleratio
	Velocity.Z += GetGravityZ() * WallSlideGravityScaleCurve->GetFloatValue(bVelUp ? 0.f : ChangeVar) * timeTick;*/

	bool bVelUp = Velocity.Z > 0.f;
	//Velocity.Z += GetGravityZ() * WallSlideGravityScaleCurve->GetFloatValue(bVelUp ? 0.f : ChangeVar) * DeltaTime;

	Acceleration = FVector::ZeroVector;

	Iterations++;
	bJustTeleported = false;

	FVector InitialLocation = UpdatedComponent->GetComponentLocation();
	FQuat InitialRotation = UpdatedComponent->GetComponentQuat();
	FVector LocationUpdate = Velocity * DeltaTime;
	FHitResult Hit(1.0f);
	SafeMoveUpdatedComponent(LocationUpdate, InitialRotation, true, Hit);

	if (Hit.Time < 1.0f)
	{
		HandleImpact(Hit, DeltaTime, LocationUpdate);
		Velocity = (UpdatedComponent->GetComponentLocation() - InitialLocation) / DeltaTime;
	}
}


bool UTwoDCMC::GetWallslideWall(TArray<FHitResult>& Hits) const
{
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = Start + ((UB_CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius() + WallslideDistanceFromWall) * UB_CharacterOwner->GetActorForwardVector());
	return GetWorld()->LineTraceMultiByProfile(Hits, Start, End, TEXT("BlockAll"), UB_CharacterOwner->GetIgnoreSelfParams());
}

bool UTwoDCMC::GetWallslideFloor(TArray<FHitResult>& Hits) const
{
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = Start + (FVector::DownVector * (UB_CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + WallslideMinFloorHeight));
	return GetWorld()->LineTraceMultiByProfile(Hits, Start, End, TEXT("BlockAll"), UB_CharacterOwner->GetIgnoreSelfParams());
}

bool UTwoDCMC::CheckWallAtLeastCapsuleHeight(const FHitResult& Hit)
{
	TArray<FHitResult> TopHits;
	TArray<FHitResult> BottomHits;
	AActor* HitActor = Hit.GetActor();

	// Check top
	FVector Start = UpdatedComponent->GetComponentLocation() + (FVector::UpVector * UB_CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	FVector End = Start + (UB_CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius() * 2 * UB_CharacterOwner->GetActorForwardVector());
	if (!GetWorld()->LineTraceMultiByProfile(TopHits, Start, End, TEXT("BlockAll"), UB_CharacterOwner->GetIgnoreSelfParams()))
	{
		return false;
	}
	bool FoundSameObject = false;
	for (const FHitResult& TopHit : TopHits)
	{
		if (TopHit.GetActor() == HitActor)
		{
			FoundSameObject = true;
			break;
		}
	}
	if (!FoundSameObject) { return false; }

	// Check bottom
	Start = UpdatedComponent->GetComponentLocation() + (FVector::DownVector * UB_CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	End = Start + (UB_CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius() * 2 * UB_CharacterOwner->GetActorForwardVector());
	if (!GetWorld()->LineTraceMultiByProfile(BottomHits, Start, End, TEXT("BlockAll"), UB_CharacterOwner->GetIgnoreSelfParams()))
	{
		return false;
	}
	FoundSameObject = false;
	for (const FHitResult& BottomHit : BottomHits)
	{
		if (BottomHit.GetActor() == HitActor)
		{
			FoundSameObject = true;
			break;
		}
	}
	if (!FoundSameObject) { return false; }

	return true;
}

bool UTwoDCMC::IsEligibleWallForSliding(FHitResult& Hit)
{
	// Check whether wall angle is within our allowable range
	if (FVector::DotProduct(Hit.Normal, -1 * UB_CharacterOwner->GetActorForwardVector()) < WallslideAllowableWallAngleStrictness)
	{
		return false;
	}

	// Check whether wall is suitably long (e.g. ignore small platforms)
	if (!CheckWallAtLeastCapsuleHeight(Hit))
	{ 
		return false; 
	}

	return true;
}

bool UTwoDCMC::IsHeadedForwards() const
{
	//If input matches forward vector
	return FVector::DotProduct(UB_CharacterOwner->GetActorForwardVector(), Acceleration.GetSafeNormal()) >= WallslideInputAngleStrictness;
}

bool UTwoDCMC::GetValidFloorBeneath(FHitResult& Hit)
{
	TArray<FHitResult> HitResultsFloor;
	if (GetWallslideFloor(HitResultsFloor))
	{
		for (FHitResult hit : HitResultsFloor)
		{
			// Bit of a hack: We are reusing the IsValidLandingSpot logic, but this method expects to be called 
			//  after an attempted movement of the component into the floor. We are calling from a trace,  
			//  so we need to adjust the effective hit impact
			hit.ImpactPoint.Z -= UB_CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			if (IsValidLandingSpot(UpdatedComponent->GetComponentLocation(), hit))
			{
				Hit = hit;
				return true;
			}
		}
	}
	return false;
}

void UTwoDCMC::PerformWallJump()
{
	LastWallJumpTime = GetWorld()->GetTimeSeconds();
	HasRecentlyWalljumped = true;
	UB_CharacterOwner->JumpCurrentCount--; // walljump does not count against jump count

	if (Safe_bFacingRight == LastWallWasRelativeRight)
	{
		RequestTurnAround();
	}
	FVector LaunchVector = WallslideJumpOffForce * FVector::ForwardVector;
	FRotator LaunchRotator(Safe_bFacingRight ? WallslideJumpAngle : WallslideJumpAngle + 90.0f, 0.0f, 0.0f);
	Launch(LaunchRotator.RotateVector(LaunchVector));

}

bool UTwoDCMC::CanWalljump() const
{
	return (IsWallsliding || HasRecentlyWallSlid) && !HasRecentlyWalljumped && WallslideJumpOffForce > 0.0f;
}
void UTwoDCMC::OnBeginWallslide_Implementation()
{
}

void UTwoDCMC::OnEndWallslide_Implementation()
{
}

void UTwoDCMC::OnRep_IsWallsliding()
{
	// not sure if this is the best way
	if (IsWallsliding)
	{
		EnterWallslide();
	}
	else
	{
		ExitWallslide();
	}
	bNetworkUpdateReceived = true;
}
#pragma endregion

void UTwoDCMC::EnterDash()
{
}

void UTwoDCMC::ExitDash(bool DashCancelled)
{
}

void UTwoDCMC::PhysDash(float DeltaTime, int32 Iterations)
{
}

void UTwoDCMC::PerformDash()
{
}

/*
class AOneWayPlatform* UTwoDCMC::FindOneWayPlatform()
{
	FFindFloorResult FloorResult;
	FindFloor(UpdatedComponent->GetComponentLocation(), FloorResult, false);
	if (FloorResult.IsWalkableFloor())
	{
		AOneWayPlatform* OneWayPlatform = Cast<AOneWayPlatform>(FloorResult.HitResult.GetActor());
		if (OneWayPlatform)
		{
			return OneWayPlatform;
		}
	}
	return nullptr;
}
*/

void UTwoDCMC::AddInputVector(FVector WorldVector, bool bForce)
{
	
	if (!MoveInputsEnabled ) { return; }

	bool IsRightMotion = WorldVector.X > 0;

	if (ShouldFreeMoveFacing() && bAimingActive)
	{
		// Aiming controls facing; never trigger a flip from movement here.
		Super::AddInputVector(WorldVector, bForce);
		return;
	}
	
	if (IsRightMotion == Safe_bFacingRight)
	{
		// This is movement in the direction we're facing, so add as normal
		Super::AddInputVector(WorldVector, bForce);
	}
	else if (!Turning && !HasRecentlyWalljumped)
	{
		// Motion in the reverse direction
		// Rather than realize this as rotation in our plane-constrained physics, 
		//  we ensure we do a full turn before adding any other input
		RequestTurnAround();
	}
}

bool UTwoDCMC::DoJump(bool bReplayingMoves)
{
	bool hanging = IsHanging();
	bool InWallslide = CanWalljump();
	bool InDash = IsDashing;
	if (Super::DoJump(bReplayingMoves))
	{
		if (hanging)
		{
			StopHanging();
			//UB_CharacterOwner->GetCapsuleComponent()->IgnoreActorWhenMoving(currLedge, true);
			Velocity += FVector::UpVector * 300 * .5f;
			Velocity += Acceleration.GetSafeNormal2D() * 400 * .5f;
		}
		if (InWallslide)
		{
			PerformWallJump();
			ExitWallslide();
			SetMovementMode(MOVE_Falling);
		}
		else if (InDash)
		{
			ExitDash(true);
			SetMovementMode(MOVE_Falling);
		}
		return true;
	}
	return false;
}

bool UTwoDCMC::CanCrouchInCurrentState() const
{
	return Super::CanCrouchInCurrentState() && IsMovingOnGround();
	//&& IsMovingOnGround();
}

void UTwoDCMC::SetFacingRight(bool bFacingRight)
{
	Safe_bFacingRight = bFacingRight;
}

bool UTwoDCMC::IsCustomMovementMode(EUBCharacterMovementMode Mode) const
{
	return (MovementMode == MOVE_Custom) && (CustomMovementMode == Mode);
	// OG (MovementMode == MOVE_Custom) && (CustomMovementMode == (uint8)Mode)  || 
}

bool UTwoDCMC::RequestTurnAround()
{
	if ( IsHanging()) { return false; }
	if (IsMovementMode(MOVE_Flying)) { return false; }
	if (Turning) { return false; }
	if (IsDashing && !CanTurnWhileDashing) { return false; }
	Turning = true;
	Safe_bFacingRight = !Safe_bFacingRight;

	

	return true;
}

void UTwoDCMC::RequestFaceDirection(float AxisX)
{
	const float Deadzone = 0.1f;

	// Aiming should only control facing when player is free-moving:
	const bool bAimingCanControlFacing =
		   (IsWalking() || IsFalling())  // grounded walking, crouching,  airborne movement
		&& !IsWallsliding
		&& !IsDashing
		&& !Turning;                     // do not interrupt ongoing turnaround

	if (!bAimingCanControlFacing)
	{
		return; // defer control to normal RequestTurnAround() triggers
	}

	if (AxisX > Deadzone && !Safe_bFacingRight)
	{
		RequestTurnAround();
	}
	else if (AxisX < -Deadzone && Safe_bFacingRight)
	{
		RequestTurnAround();
	}
}


bool UTwoDCMC::CanDash() const
{
	return false;
}

void UTwoDCMC::OnBeginDash_Implementation()
{
}

void UTwoDCMC::OnEndDash_Implementation()
{
}

void UTwoDCMC::OnRep_IsDashing()
{
}




#pragma endregion

#pragma region BusterMovementInitiationFunctions

void UTwoDCMC::SprintPressed()
{
	if (!IsCrouching()) Safe_bWantsToSprint = true;
	
}
void UTwoDCMC::SprintReleased()
{
	Safe_bWantsToSprint = false;
}

void UTwoDCMC::CrouchPressed()
{

  //bWantsToCrouch = !bWantsToCrouch;
	bWantsToCrouch = true;
	//GetWorld()->GetTimerManager().SetTimer(TimerHandle_EnterProne, this, &UTwoDCMC::OnTryEnterProne, ProneEnterHoldDuration);
}

void UTwoDCMC::CrouchReleased()
{
//bWantsToCrouch = !bWantsToCrouch;
	bWantsToCrouch = false;
	//GetWorld()->GetTimerManager().ClearTimer(TimerHandle_EnterProne);
	//Safe_bWantsToProne = false; 
}

bool UTwoDCMC::CanSlide() const
{
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = Start + CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.5f * FVector::DownVector;
	FName ProfileName = TEXT("BlockAll");
	bool bValidSurface = GetWorld()->LineTraceTestByProfile(Start, End, ProfileName, UB_CharacterOwner->GetIgnoreSelfParams());
	bool bEnoughSpeed = Velocity.SizeSquared() > pow(MinSlideSpeed, 2);

	return bValidSurface && bEnoughSpeed;
}

void UTwoDCMC::PhysSlide(float deltaTime, int32 Iterations)
{
	
    
	if (deltaTime < MIN_TICK_TIME)
	{
		
		return;
	}


	if (!CanSlide())
	{
		
		SetMovementMode(MOVE_Walking);
		StartNewPhysics(deltaTime, Iterations);
		return;
	}

	bJustTeleported = false;
	bool bCheckedFall = false;
	bool bTriedLedgeMove = false;
	float remainingTime = deltaTime;

	// Perform the move
	while ( (remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)) )
	{

		
		Iterations++;
		bJustTeleported = false;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		// Save current values
		UPrimitiveComponent * const OldBase = GetMovementBase();
		const FVector PreviousBaseLocation = (OldBase != NULL) ? OldBase->GetComponentLocation() : FVector::ZeroVector;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FFindFloorResult OldFloor = CurrentFloor;

		// Ensure velocity is horizontal.
		MaintainHorizontalGroundVelocity();
		const FVector OldVelocity = Velocity;

		FVector SlopeForce = CurrentFloor.HitResult.Normal;
		SlopeForce.Z = 0.f;
		Velocity += SlopeForce * SlideGravityForce * deltaTime;
		// deltaTime was in timetick spot

		Acceleration = Acceleration.ProjectOnTo(UpdatedComponent->GetRightVector().GetSafeNormal2D());

		// Apply acceleration
		CalcVelocity(timeTick, GroundFriction * SlideFrictionFactor, false, GetMaxBrakingDeceleration());

		// Compute move parameters
		const FVector MoveVelocity = Velocity;
		const FVector Delta = timeTick * MoveVelocity;
		const bool bZeroDelta = Delta.IsNearlyZero();
		FStepDownResult StepDownResult;
		bool bFloorWalkable = CurrentFloor.IsWalkableFloor();

		if ( bZeroDelta )
		{
			remainingTime = 0.f;
		}
		else
		{
			// try to move forward
			MoveAlongFloor(MoveVelocity, timeTick, &StepDownResult);

			if ( IsFalling() )
			{
				
				// pawn decided to jump up
				const float DesiredDist = Delta.Size();
				if (DesiredDist > KINDA_SMALL_NUMBER)
				{
					const float ActualDist = (UpdatedComponent->GetComponentLocation() - OldLocation).Size2D();
					remainingTime += timeTick * (1.f - FMath::Min(1.f,ActualDist/DesiredDist));
				}
				StartNewPhysics(remainingTime,Iterations);
				return;
			}
			else if ( IsSwimming() ) //just entered water
			{
				StartSwimming(OldLocation, OldVelocity, timeTick, remainingTime, Iterations);
				return;
			}
		}

		// Update floor.
		// StepUp might have already done it for us.
		if (StepDownResult.bComputedFloor)
		{
			
			CurrentFloor = StepDownResult.FloorResult;
		}
		else
		{
			FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, bZeroDelta, NULL);
		}


		// check for ledges here
		const bool bCheckLedges = !CanWalkOffLedges();
		if ( bCheckLedges && !CurrentFloor.IsWalkableFloor() )
		{
			// calculate possible alternate movement
			const FVector GravDir = FVector(0.f,0.f,-1.f);
			const FVector NewDelta = bTriedLedgeMove ? FVector::ZeroVector : GetLedgeMove(OldLocation, Delta, OldFloor);
			if ( !NewDelta.IsZero() )
			{
				// first revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, false);

				// avoid repeated ledge moves if the first one fails
				bTriedLedgeMove = true;

				// Try new movement direction
				Velocity = NewDelta / timeTick;
				remainingTime += timeTick;
				continue;
			}
			else
			{
				// see if it is OK to jump
				// @todo collision : only thing that can be problem is that oldbase has world collision on
				bool bMustJump = bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ( (bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump) )
				{
					return;
				}
				bCheckedFall = true;

				// revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, true);
				remainingTime = 0.f;
				break;
			}
		}
		else
		{
			// Validate the floor check
			if (CurrentFloor.IsWalkableFloor())
			{
				if (ShouldCatchAir(OldFloor, CurrentFloor))
				{
					HandleWalkingOffLedge(OldFloor.HitResult.ImpactNormal, OldFloor.HitResult.Normal, OldLocation, timeTick);
					if (IsMovingOnGround())
					{
						// If still walking, then fall. If not, assume the user set a different mode they want to keep.
						StartFalling(Iterations, remainingTime, timeTick, Delta, OldLocation);
					}
					return;
				}

				AdjustFloorHeight();
				SetBase(CurrentFloor.HitResult.Component.Get(), CurrentFloor.HitResult.BoneName);
			}
			else if (CurrentFloor.HitResult.bStartPenetrating && remainingTime <= 0.f)
			{
				// The floor check failed because it started in penetration
				// We do not want to try to move downward because the downward sweep failed, rather we'd like to try to pop out of the floor.
				FHitResult Hit(CurrentFloor.HitResult);
				Hit.TraceEnd = Hit.TraceStart + FVector(0.f, 0.f, MAX_FLOOR_DIST);
				const FVector RequestedAdjustment = GetPenetrationAdjustment(Hit);
				ResolvePenetration(RequestedAdjustment, Hit, UpdatedComponent->GetComponentQuat());
				bForceNextFloorCheck = true;
			}

			// check if just entered water
			if ( IsSwimming() )
			{
				StartSwimming(OldLocation, Velocity, timeTick, remainingTime, Iterations);
				return;
			}

			// See if we need to start falling.
			if (!CurrentFloor.IsWalkableFloor() && !CurrentFloor.HitResult.bStartPenetrating)
			{
				const bool bMustJump = bJustTeleported || bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump) )
				{
					return;
				}
				bCheckedFall = true;
			}
		}

		// Allow overlap events and such to change physics state and velocity
		if (IsMovingOnGround() && bFloorWalkable)
		{
			// Make velocity reflect actual move
			if( !bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && timeTick >= MIN_TICK_TIME)
			{
				// TODO-RootMotionSource: Allow this to happen during partial override Velocity, but only set allowed axes?
				Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick;
				MaintainHorizontalGroundVelocity();
			}
		}

		// If we didn't move at all this iteration then abort (since future iterations will also be stuck).
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			
			remainingTime = 0.f;
			break;
		}
	}



	FHitResult Hit;
	FQuat NewRotation = FRotationMatrix::MakeFromXZ(Velocity.GetSafeNormal2D(), FVector::UpVector).ToQuat();
	SafeMoveUpdatedComponent(FVector::ZeroVector, NewRotation, false, Hit);
}

void UTwoDCMC::EnterSlide(EMovementMode PreviousMovementMode, EUBCharacterMovementMode PreviousCustomMode)
{
	bWantsToCrouch = true;
	Crouch(false);
	//Velocity += Velocity.GetSafeNormal2D() * SlideEnterImpulse;

	FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, true, NULL);
}

void UTwoDCMC::ExitSlide()
{
	
	//bWantsToCrouch = false;
	
}

void UTwoDCMC::UpMoveInputPressed()
{
	if (IsHanging())
	{
		Safe_bWantsToMantle = true;
	}
}

#pragma endregion

#pragma region Helper Functions

void UTwoDCMC::FSavedMove_UltraBuster::getAttachedBusterMovement(ACharacter* C, UTwoDCMC*& CharacterMovement)
{
	CharacterMovement = Cast<UTwoDCMC>(C->GetCharacterMovement());
}

bool UTwoDCMC::IsMovementMode(EMovementMode InMovementMode) const
{
	return MovementMode == InMovementMode;
}

bool UTwoDCMC::TryLedgeGrab()
{
	// could just set to climb here
	//probably need make rotation 54:33
		// Temp Enable Crouching

	// Only enable while falling
	if ( !IsMovementMode(MOVE_Falling)) return false;

	// Helper Variables
	FVector BaseLoc = UpdatedComponent->GetComponentLocation() + FVector::DownVector * CapHH();
	FVector Fwd = UpdatedComponent->GetForwardVector().GetSafeNormal2D();
	auto Params = UB_CharacterOwner->GetIgnoreSelfParams();
	float MaxHeight = CapHH() * 2 + LedgeGrabReachHeight;
	float CosMMWSA = FMath::Cos(FMath::DegreesToRadians(LedgeGrabMinWallSteepnessAngle));
	float CosMMSA = FMath::Cos(FMath::DegreesToRadians(LedgeGrabMaxSurfaceAngle));
	float CosMMAA = FMath::Cos(FMath::DegreesToRadians(LedgeGrabMaxAlignmentAngle));

	

	// Check Front Face
	FHitResult FrontHit;

	// How Far From Ledge Before you can Init ledge Grab
	float CheckDistance = FMath::Clamp(Velocity | Fwd, CapR() + 30, MaxLedgeGrabDistance);
	
	FVector FrontStart = BaseLoc + FVector::UpVector * (MaxStepHeight - 1);
	for (int i = 0; i < 6; i++)
	{
		//LINE(FrontStart, FrontStart + Fwd * CheckDistance, FColor::Red)
		if (GetWorld()->LineTraceSingleByProfile(FrontHit, FrontStart, FrontStart + Fwd * CheckDistance, "BlockAll", Params)) break;
		FrontStart += FVector::UpVector * (2.f * CapHH() - (MaxStepHeight - 1)) / 5;
	}
	if (!FrontHit.IsValidBlockingHit()) return false;
	float CosWallSteepnessAngle = FrontHit.Normal | FVector::UpVector;
	// Pipe Symbol is used for the dot product in this context
	if (FMath::Abs(CosWallSteepnessAngle) > CosMMWSA || (Fwd | -FrontHit.Normal) < CosMMAA) return false;

	//POINT(FrontHit.Location, FColor::Red);

	// Check Height
	TArray<FHitResult> HeightHits;
	FHitResult SurfaceHit;
	// Vector traveling in the direction up the surface the wall to the edge 
	FVector WallUp = FVector::VectorPlaneProject(FVector::UpVector, FrontHit.Normal).GetSafeNormal();
	//Angle between world up and WallUP
	float WallCos = FVector::UpVector | FrontHit.Normal;

	//enbales us to make a downward cast down towards the wall
	float WallSin = FMath::Sqrt(1 - WallCos * WallCos);
	// 
	FVector TraceStart = FrontHit.Location + Fwd + WallUp * (MaxHeight - (MaxStepHeight - 1)) / WallSin;
	//LINE(TraceStart, FrontHit.Location + Fwd, FColor::Orange)
		if (!GetWorld()->LineTraceMultiByProfile(HeightHits, TraceStart, FrontHit.Location + Fwd, "BlockAll", Params)) return false;
	for (const FHitResult& Hit : HeightHits)
	{
		if (Hit.IsValidBlockingHit())
		{
			SurfaceHit = Hit;
			break;
		}
	}
	// if no blocking hit or surface is too steep
	if (!SurfaceHit.IsValidBlockingHit() || (SurfaceHit.Normal | FVector::UpVector) < CosMMSA) return false;
	float Height = (SurfaceHit.Location - BaseLoc) | FVector::UpVector;



	if (Height > MaxHeight) return false;

	// Check Clearance
	float SurfaceCos = FVector::UpVector | SurfaceHit.Normal;
	float SurfaceSin = FMath::Sqrt(1 - SurfaceCos * SurfaceCos);
	FVector ClearCapLoc = SurfaceHit.Location + Fwd * CapR() + FVector::UpVector * (CapHH() + 1 + CapR() * 2 * SurfaceSin);
	FCollisionShape CapShape = FCollisionShape::MakeCapsule(CapR(), CapHH());
	if (GetWorld()->OverlapAnyTestByProfile(ClearCapLoc, FQuat::Identity, "BlockAll", CapShape, Params))
	{
		//CAPSULE(ClearCapLoc, FColor::Red)
				return false;
	}
	else
	{
		//CAPSULE(ClearCapLoc, FColor::Green)
	}

	
	// LedgeGrab Selection
	//FVector ShortLedgeGrabTarget = GetLedgeGrabStartLocation(FrontHit, SurfaceHit, false);
	TallLedgeGrabTarget = GetLedgeGrabStartLocation(FrontHit, SurfaceHit);
	
	bool bTallLedgeGrab = false;
	if (IsMovementMode(MOVE_Falling) && Height > CapHH())
		bTallLedgeGrab = true;
	else if (IsMovementMode(MOVE_Falling) && (Velocity | FVector::UpVector) < 0)
	{
		if (!GetWorld()->OverlapAnyTestByProfile(TallLedgeGrabTarget, FQuat::Identity, "BlockAll", CapShape, Params))
			bTallLedgeGrab = true;
	}

	/*
	FVector ForwardOffset = UpdatedComponent->GetForwardVector() * LedgeGrabXOffset;
	FVector UpOffset = FVector::UpVector * LedgeGrabZOffset;
	FVector TransitionTarget = TallLedgeGrabTarget + ForwardOffset + UpOffset;
	*/


	FVector TransitionTarget = TallLedgeGrabTarget;

	//  Exit Early if Capsule Does not fit
	// Test if character can reach goal
	// Technically this moves the character in one frame and then decides if they 
	FTransform CurrentTransform = UpdatedComponent->GetComponentTransform();
	FHitResult Hit, ReturnHit;
	SafeMoveUpdatedComponent(TransitionTarget - UpdatedComponent->GetComponentLocation(), UpdatedComponent->GetComponentQuat(), true, Hit); //bsweep was true
	FVector ResultLocation = UpdatedComponent->GetComponentLocation();
	SafeMoveUpdatedComponent(CurrentTransform.GetLocation() - ResultLocation, UpdatedComponent->GetComponentQuat(), false, ReturnHit);

	currLedge = SurfaceHit.GetActor();
	Turning = true;

	
	if (!ResultLocation.Equals(TransitionTarget, 2.f))  // The two is tolerance since some rotations mess up the calculations
	{
		
		return false;
	}

	// Perform Transition to LedgeGrab
	


	float UpSpeed = Velocity | FVector::UpVector;
	float TransDistance = FVector::Dist(TransitionTarget, UpdatedComponent->GetComponentLocation());

	TransitionQueuedMontageSpeed = FMath::GetMappedRangeValueClamped(FVector2D(-500, 750), FVector2D(.9f, 1.2f), UpSpeed);
	TransitionRMS.Reset();
	// Make new RMS Struct
	TransitionRMS = MakeShared<FRootMotionSource_MoveToForce>();
	// Overide Rather than add, This is probably covered by reset
	TransitionRMS->AccumulateMode = ERootMotionAccumulateMode::Override;

	// Set duration based on distance to target. May need to be scaled up or down given current values between .1 and .25
	TransitionRMS->Duration = FMath::Clamp(TransDistance / 500.f, .1f, .5f);
	

	TransitionRMS->StartLocation = UpdatedComponent->GetComponentLocation();
	TransitionRMS->TargetLocation = TransitionTarget;

	// Apply Transition Root Motion Source
	Velocity = FVector::ZeroVector;
	// Flying helps with three dimensional root motion but I may be able to do it with a custom climbing mode
	SetMovementMode(MOVE_Flying);
	TransitionRMS_ID = ApplyRootMotionSource(TransitionRMS);

	TransitionName = "LedgeGrab";
	
	// cache the exact ledge direction for PhysHang:
	//CurrentLedgeTangent = FVector::CrossProduct(SurfaceHit.Normal, FrontHit.Normal).GetSafeNormal();

	
	// Animations
		//TransitionQueuedMontage = TallLedgeGrabMontage;
		// Transition is not a root motion montage but maybe I can see how this would work with motion warping
		CharacterOwner->PlayAnimMontage(TallLedgeGrabMontage, 1 / TransitionRMS->Duration); ///  /
	
	// Make sure clients get this ASAP
	
		//CharacterOwner->ForceNetUpdate();
	

	// temp remove experiment
//	if (IsServer()) Proxy_bLedgeGrabbed = !Proxy_bLedgeGrabbed;
		
	

	return true;
}

void UTwoDCMC::PressedGrabLedge()
{
	Safe_bWantsToMantle = true;
}

bool UTwoDCMC::TryMantle()
{
	
	if (!IsCustomMovementMode(UBMOVE_Hang)) return false;
	
	// Helper Variables
	// Baser Loc At Character Eye Height
	FVector BaseLoc = UpdatedComponent->GetComponentLocation() + UpdatedComponent->GetUpVector() * CharacterOwner->BaseEyeHeight;
	// Direction Vector
	FVector Fwd = UpdatedComponent->GetForwardVector().GetSafeNormal2D();
	// Ignore Character and Components
	auto Params = UB_CharacterOwner->GetIgnoreSelfParams();
	//probably irrelevant for mantle
	float MaxHeight = CapHH() * 2 + LedgeGrabReachHeight;

	
	float CosMMWSA = FMath::Cos(FMath::DegreesToRadians(LedgeGrabMinWallSteepnessAngle));
	float CosMMSA = FMath::Cos(FMath::DegreesToRadians(LedgeGrabMaxSurfaceAngle));
	float CosMMAA = FMath::Cos(FMath::DegreesToRadians(LedgeGrabMaxAlignmentAngle));
	

	// distance for eye line trace
	const FVector WallEnd = BaseLoc + Fwd * MaxLedgeGrabDistance;
	
	// Check Front Face
	FHitResult FrontHit;

	//removed the velocity clamp
	//Simply Check forward 20 units
	float CheckDistance = 50.f;
	FVector FrontStart = BaseLoc;

	//Do five traces from the center of the body to hands
	for (int i = 0; i < 6; i++)
	{
		//LINE(FrontStart, FrontStart + Fwd * CheckDistance, FColor::Red)
		if (GetWorld()->LineTraceSingleByProfile(FrontHit, FrontStart, FrontStart + Fwd * CheckDistance, "BlockAll", Params)) break;
		// Ideally I would be performing 5 traces from the center of the body to the hand roughly
		FrontStart += (FVector::UpVector * 2)  / 5;
	}
	// return if no hits. (Probably only possible if I am on a small railing)
	if (!FrontHit.IsValidBlockingHit()) return false;

//POINT(FrontHit.Location, FColor::Red);
	
	// Check Height
	TArray<FHitResult> HeightHits;
	FHitResult SurfaceHit;
// Project up vector onto the wall plane
	FVector WallUp = FVector::VectorPlaneProject(FVector::UpVector, FrontHit.Normal).GetSafeNormal();

	// angle between upvector and hit normal in radians
	float WallCos = FVector::UpVector | FrontHit.Normal;
	
	float WallSin = FMath::Sqrt(1 - WallCos * WallCos);
	
	FVector TraceStart = FrontHit.Location + Fwd + WallUp * (MaxHeight - (MaxStepHeight - 1)) / WallSin;
	//LINE(TraceStart, FrontHit.Location + Fwd, FColor::Orange)
	if (!GetWorld()->LineTraceMultiByProfile(HeightHits, TraceStart, FrontHit.Location + Fwd, "BlockAll", Params)) return false;
	for (const FHitResult& Hit : HeightHits)
	{
		if (Hit.IsValidBlockingHit())
		{
			SurfaceHit = Hit;
			break;
		}
	}
	//return if the top of the surface has too great of a slope
	if (!SurfaceHit.IsValidBlockingHit() || (SurfaceHit.Normal | FVector::UpVector) < CosMMSA) return false;
	float Height = (SurfaceHit.Location - BaseLoc) | FVector::UpVector;

	POINT(SurfaceHit.Location, FColor::Blue);
	
	
	// Check Clearance
	float SurfaceCos = FVector::UpVector | SurfaceHit.Normal;
	float SurfaceSin = FMath::Sqrt(1 - SurfaceCos * SurfaceCos);
	FVector ClearCapLoc = SurfaceHit.Location + Fwd * CapR() + FVector::UpVector * (CapHH() + 1 + CapR() * 2 * SurfaceSin);
	FCollisionShape CapShape = FCollisionShape::MakeCapsule(CapR(), CapHH());
	if (GetWorld()->OverlapAnyTestByProfile(ClearCapLoc, FQuat::Identity, "BlockAll", CapShape, Params))
	{
	//CAPSULE(ClearCapLoc, FColor::Red)
		return false;
	}
	else
	{
	//CAPSULE(ClearCapLoc, FColor::Green)
	}
	
	

	return true;
}
void UTwoDCMC::PhysHang(float deltaTime, int32 Iterations)
{
	// if no ledge data recieved
	//if (TallLedgeGrabTarget.IsNearlyZero()) return;
	SLOG("Entered Hang")
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	// No physics until replicated anchor is ready.
	const bool bHasAnchor   = !TallLedgeGrabTarget.IsNearlyZero();
	const bool bHasNormal   = !HangNormal.IsNearlyZero();

	if (!bHasAnchor || !bHasNormal)
	{
		// Freeze in place – don’t nudge off the ledge while we wait.
		Acceleration = FVector::ZeroVector;
		Velocity     = FVector::ZeroVector;
		return;
	}

	
	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}
	/*
	//GrabAcceleration
	FVector Input = GetPendingInputVector().GetClampedToMaxSize(1.f);
	FVector LastInput = ConsumeInputVector();
	Acceleration = ScaleInputAcceleration(Input);
	*/

	
	// Rotation to face wall
	const FQuat HangRotation = FRotationMatrix::MakeFromXZ(-HangNormal, FVector::UpVector).ToQuat();
	
	// Snap to anchor (no sweep so physics won’t push us off)
	FHitResult Hit;
	if (!TallLedgeGrabTarget.IsNearlyZero())
	{
		SafeMoveUpdatedComponent(TallLedgeGrabTarget - UpdatedComponent->GetComponentLocation(), HangRotation, false, Hit);
	}
	else
	{
		SafeMoveUpdatedComponent(
		TallLedgeGrabTarget - UpdatedComponent->GetComponentLocation(),
		HangRotation,
		false,
		Hit
		);
	}
		
	Acceleration = FVector::ZeroVector;
	Velocity = FVector::ZeroVector;


	if (bWantsToCrouch)
	{
		StopHanging();
		//EnterWallslide();
	}

	FVector Input = GetPendingInputVector().GetClampedToMaxSize(1.f);

	if (GetCurrentAcceleration().Z)
	{

		Safe_bWantsToMantle = true;

	}
}

FVector UTwoDCMC::GetLedgeGrabStartLocation(FHitResult FrontHit, FHitResult SurfaceHit) 
{
	float CosWallSteepnessAngle = FrontHit.Normal | FVector::UpVector;
	//Probably needs adjusting
	float DownDistance =  CapHH() * 2.f;
	FVector EdgeTangent = FVector::CrossProduct(SurfaceHit.Normal, FrontHit.Normal).GetSafeNormal();

	FVector LedgeGrabStart = SurfaceHit.Location;
	HangNormal = FrontHit.Normal.GetSafeNormal2D();
	LedgeGrabStart += FrontHit.Normal.GetSafeNormal2D() * (2.f + CapR());
	LedgeGrabStart += UpdatedComponent->GetForwardVector().GetSafeNormal2D().ProjectOnTo(EdgeTangent) * CapR() * .3f;
	LedgeGrabStart += FVector::UpVector * CapHH();
	LedgeGrabStart += FVector::DownVector * DownDistance;
	LedgeGrabStart += FrontHit.Normal.GetSafeNormal2D() * CosWallSteepnessAngle * DownDistance;

	return LedgeGrabStart;
}


void UTwoDCMC::StopHanging()
{
	SetMovementMode(MOVE_Falling);
}

bool UTwoDCMC::IsServer() const
{
	return CharacterOwner->HasAuthority();
}

float UTwoDCMC::CapR() const
{
	return CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();
}

float UTwoDCMC::CapHH() const
{
	return CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}

void UTwoDCMC::OnRep_LedgeGrab()
{
	if (TallLedgeGrabTarget.IsNearlyZero() || HangNormal.IsNearlyZero())
		return;

	const FQuat Rot = FRotationMatrix::MakeFromXZ(-HangNormal, FVector::UpVector).ToQuat();
	UpdatedComponent->SetWorldLocationAndRotation(TallLedgeGrabTarget, Rot, /*bSweep=*/false);

	// Lock in place so client doesn’t drift before first PhysHang tick
	Velocity     = FVector::ZeroVector;
	Acceleration = FVector::ZeroVector;
}

void UTwoDCMC::SetAimInput(const FVector2D& Aim2D, float Deadzone)
{
	const float mag = Aim2D.Size();
	bAimingActive = (mag > Deadzone);

	if (!bAimingActive) return;

	// Decide only when X is clearly left/right
	if (Aim2D.X > Deadzone)      bAimWantsRight = true;
	else if (Aim2D.X < -Deadzone) bAimWantsRight = false;
	// If |X| ≤ Deadzone, leave last bAimWantsRight as-is
}

void UTwoDCMC::ApplyAimFacing()
{
	if (!bAimingActive) return;
	if (!ShouldFreeMoveFacing()) return;

	if (bAimWantsRight != Safe_bFacingRight)
	{
		RequestTurnAround();
	}
}

bool UTwoDCMC::ShouldFreeMoveFacing() const
{
	return (IsWalking() || IsFalling()) && !IsWallsliding && !IsDashing && !Turning && !Safe_bWantsToSprint;
}
#pragma endregion
