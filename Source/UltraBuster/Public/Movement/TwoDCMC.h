// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TwoDCMC.generated.h"

UENUM(BlueprintType)
enum EUBCharacterMovementMode
{
	UBMOVE_NONE UMETA(Hidden),
	UBMOVE_WALLSLIDING   UMETA(DisplayName = "Wallsliding"),
	UBMOVE_WallRun UMETA(DisplayName = "WallRun"),
	UBMOVE_DASHING	UMETA(DisplayName = "Dashing"),
	UBMOVE_Slide UMETA(DisplayName = "Slide"),
	UBMOVE_Prone UMETA(DisplayName = "Prone"),
	UBMOVE_Hang	 UMETA(DisplayName = "Hang"),
	UBMOVE_MAX      UMETA(Hidden)
};

/**
 * 
 */

// ZIPDelegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDashStartDelegate);

//SSTDelegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPerformDashDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndDashDelegate);

UCLASS()
class ULTRABUSTER_API UTwoDCMC : public UCharacterMovementComponent
{
	GENERATED_BODY()

#pragma  region SavedMove_and_PredictionData
	//Snap Shot Of all the data that is required to produce a move
	class FSavedMove_UltraBuster : public FSavedMove_Character
	{
	public:
		// For Movement States That need TO be updated regularly
		// State Replicated from server to client
		enum CompressedFlags
		{
			FLAG_Sprint			= 0x10,
			FLAG_Dash			= 0x20,
			FLAG_Right			= 0x40,
			FLAG_Custom_3		= 0x80,
		};
		
		// Flags
		// These are what plays the move
		uint8 Saved_bWantsToSprint:1;
		uint8 Saved_bWantsToDash:1;
		// flag used to attempt to run custom jump logic before default jump logic
		uint8 Saved_bPressedUltraBusterJump:1;

		// Other Variables
		// Possible for more of these to cause desysnc since they are derived from other variables?
		uint8 Saved_bPrevWantsToCrouch:1;
		uint8 Saved_bWantsToProne:1;

		//might need to update this to 0
		uint8 Saved_bFacingRight:1;

		// flags dealing with animations like mantle dash and hang
		uint8 Saved_bHadAnimRootMotion:1;
		uint8 Saved_bTransitionFinished:1;

		uint8 Saved_bWantsToMantle:1;

		FSavedMove_UltraBuster();

		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		virtual void Clear() override;
		virtual uint8 GetCompressedFlags() const override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;
		//Optional Readability Helper
		void getAttachedBusterMovement(ACharacter* C, UTwoDCMC*& CharacterMovement);
	};

	class FNetworkPredictionData_Client_UltraBuster : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_Client_UltraBuster(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		virtual FSavedMovePtr AllocateNewMove() override;
	};

#pragma endregion
public:
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
#pragma region Component_Overrides
	UTwoDCMC();
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void InitializeComponent() override;
	virtual bool IsMovingOnGround() const override;
	virtual bool CanCrouchInCurrentState() const override;
	

#pragma endregion 

	// Transient
	UPROPERTY(Transient)
	class AUB_Character* UB_CharacterOwner;
	//save moves determine what is played
	UPROPERTY(Replicated, BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	bool Safe_bWantsToSprint;
	
	bool Safe_bWantsToProne;
	bool Safe_bPrevWantsToCrouch;
	bool Safe_bWantsToDash;
	UPROPERTY(replicated)
	bool Safe_bFacingRight;

	bool Safe_bHadAnimRootMotion;
	bool Safe_bTransitionFinished;
	bool Safe_bWantsToMantle;
	float DashStartTime;
	FTimerHandle TimerHandle_EnterProne;
	FTimerHandle TimerHandle_DashCooldown;

	TSharedPtr<FRootMotionSource_MoveToForce> TransitionRMS;
	UPROPERTY(Transient) UAnimMontage* TransitionQueuedMontage;
	float TransitionQueuedMontageSpeed;
	FString TransitionName;
	int TransitionRMS_ID;

	UPROPERTY(ReplicatedUsing=OnRep_LedgeGrab)
	bool Proxy_bLedgeGrabbed;


	UPROPERTY(Replicated)
	FVector TallLedgeGrabTarget;

	UPROPERTY(Replicated)
	FVector HangNormal;

	AActor* currLedge;
	
#pragma region CharacterMovementOverrides

	// CMC Overide
	protected:
		virtual void UpdateFromCompressedFlags(uint8 Flags) override;
		
		virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
		virtual void UpdateCharacterStateAfterMovement(float DeltaSeconds) override;
		
		virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
		virtual void PhysCustom(float deltaTime, int32 Iterations) override;
		virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

		//sst additions
		virtual void StartNewPhysics(float deltaTime, int32 Iterations) override;
		virtual float GetMaxSpeed() const override;

		virtual float GetMaxBrakingDeceleration() const override;

		virtual bool CanAttemptJump() const override;

		virtual void ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations) override;
	   //sst additions end
		UFUNCTION(BlueprintCallable)
		FString GetMovementModeAsString(EMovementMode MoveMode, uint8 CustomMode) const;

#pragma endregion

#pragma  region SST

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	protected:

	/** Time it takes to fully turn around, in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float TurnAroundSeconds = 0.12f;

	// Wallslide parameters
	/** Determines whether the character is capable of wallsliding. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool CanWallSlide = true;

	/** Speed at which the character slides down a wall */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float WallslideSpeed = 200.0f;

	/** (0-1) Controls how strictly along the x-axis the player must hold an analog input to continue wallsliding */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float WallslideInputAngleStrictness = 0.2f;

	/** (0-1) Lower values allow a wall greater deviations from pure vertical before it is no longer eligible for wallslide */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float WallslideAllowableWallAngleStrictness = .8f;

	/** In units, the distance from the wall at which the character slides */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float WallslideDistanceFromWall = 5.0f;

	/** In units, the distance above the floor before landing and ending the wallslide */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float WallslideMinFloorHeight = 5.0f;

	/** The force with which the character jumps out of a wallslide. Set to 0 to disable walljump. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float WallslideJumpOffForce = 800.0f;

	/** The angle with which the character jumps out of a wallslide. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float WallslideJumpAngle = 45.0f;

	/** Time after performing a walljump before the jump button can be used to double-jump. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float WallslideJumpInputDisableSeconds = 0.2f;

	/** Buffer after releasing a wallslide and beginning falling during which a jump will still count as a walljump. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float WallslideReleaseWalljumpBufferSeconds = 0.2f;

	/** Minimum amount of time between walljumps. Should be larger than WallslideReleaseWalljumpBufferSeconds. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float WalljumpCooldownSeconds = 0.5f;

	/** If false, dashing is disabled. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool DashEnabled = true;

	/** Number of dashes a character can perform in midair before landing. No limit if negative. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	int MaxDashesBeforeLanding = 1;

	/** In units, length of a dash. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float DashDistance = 2000;

	/** In seconds, duration of a dash. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float DashDurationSeconds = .2;

	/** Cooldown between dashes. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float DashCooldownSeconds = .7;

	/** Friction applied during dashes. Can be modified via blueprint e.g. if in water.  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float DashFriction = 0.0f;

	/** If true, the player can turn around while in the middle of a dash (changing direction of dash) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool CanTurnWhileDashing = false;

	/** If true, the player can jump while in the middle of a dash, immediately ending it */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool CanJumpCancelDash = true;

	/** When dashing on a slope facing downwards, the character will go down the slope if true, or horizontally outward if false.  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool DashFollowsDownwardSlopes = true;

public:
	/** If true, attempt to dash on next update. */
	UPROPERTY(Category = Movement, VisibleInstanceOnly, BlueprintReadOnly)
	bool WantsToDash = false;

	/** Set by character movement to specify that this Character is currently dashing. */
	UPROPERTY(BlueprintReadOnly, replicatedUsing = OnRep_IsDashing, Category = Character)
	uint32 IsDashing : 1;

	/** Blueprint-assignable dash implementation */
	UPROPERTY(BlueprintAssignable)
	FPerformDashDelegate PerformDashDelegate;
	UPROPERTY(BlueprintAssignable)
	FEndDashDelegate EndDashDelegate;

	/** Set by character movement to specify that this Character is currently wallsliding. */
	UPROPERTY(BlueprintReadOnly, replicatedUsing = OnRep_IsWallsliding, Category = Character)
	uint32 IsWallsliding : 1;

	/** If true, attempt to drop through platform on next update. */
	UPROPERTY(Category = Movement, VisibleInstanceOnly, BlueprintReadOnly)
	bool WantsToPlatformDrop = false;



	UFUNCTION(BlueprintCallable)
	void SprintPressed();
	UFUNCTION(BlueprintCallable)
	void SprintReleased();
	void CrouchPressed();
	void CrouchReleased();
	bool CanSlide() const;
	void PhysSlide(float deltaTime, int32 Iterations);
	void EnterSlide(EMovementMode PrevMode, EUBCharacterMovementMode PrevCustomMode);
	void ExitSlide();

	void UpMoveInputPressed();

private:
	UPROPERTY(replicated)
	bool FacingRight;

	bool MoveInputsEnabled = true;
	bool Turning = false;

	bool HasRecentlyWallSlid = false;
	bool HasRecentlyWalljumped = false;
	float LastWallJumpTime = 0;
	float LastWallslideFallingReleaseTime = 0;
	bool LastWallWasRelativeRight;
	uint8 CurrentDashesBeforeLanding = 0;
	float LastDashTime = 0;

	float LastDropThroughPlatformTime = 0.0f;

	// Slide
	UPROPERTY(EditDefaultsOnly)
	float MinSlideSpeed=400.f;
	UPROPERTY(EditDefaultsOnly)
	float MaxSlideSpeed=400.f;
	UPROPERTY(EditDefaultsOnly)
	float SlideEnterImpulse=400.f;
	UPROPERTY(EditDefaultsOnly)
	float SlideGravityForce=4000.f;
	UPROPERTY(EditDefaultsOnly)
	float SlideFrictionFactor=.06f;
	UPROPERTY(EditDefaultsOnly)
	float BrakingDecelerationSliding=1000.f;

	//// WallSlide
	UPROPERTY(EditDefaultsOnly)
	float WallSlideGravityForce=.5f;
	//Gravity based on entry velocity
	UPROPERTY(EditDefaultsOnly) UCurveFloat* WallSlideGravityScaleCurve;

	float InitialWallSlideZ;




public:
	/** Begin wallslide */
	void EnterWallslide();
private:

	


	/** Finish wallslide */
	void ExitWallslide();

	/** Apply physics for wallslide */
	void PhysWallslide(float DeltaTime, int32 Iterations);

	/** Trace to find wall hits */
	bool GetWallslideWall(TArray<FHitResult>& Hits) const;

	/** Trace to find floor hits */
	bool GetWallslideFloor(TArray<FHitResult>& Hits) const;

	/** Check whether a candidate wall object is greater than our capsule height */
	bool CheckWallAtLeastCapsuleHeight(const FHitResult& Hit);

	/** Verify whether a given wallslide trace result is appropriate for wallsliding */
	bool IsEligibleWallForSliding(FHitResult& Hit);

	/** Returns whether the player is holding in the direction they are facing */
	bool IsHeadedForwards() const;

	/** Helper function to check whether a wallslide is occurring just above valid floor */
	bool GetValidFloorBeneath(FHitResult& Hit);

	/** Performs wall jump */
	void PerformWallJump();

	/** Begin dash */
	void EnterDash();

	/** Finish dash */
	void ExitDash(bool DashCancelled = false);

	/** Apply physics for dash */
	void PhysDash(float DeltaTime, int32 Iterations);

	/** Performs the dash */
	void PerformDash();

	/** Determines whether the player is standing on a OneWayPlatform */
	/*class AOneWayPlatform* FindOneWayPlatform();*/

public:
	/** Overrides to enable custom movement modes */
	virtual void AddInputVector(FVector WorldVector, bool bForce = false) override;

	virtual bool DoJump(bool bReplayingMoves) override;
	

	/** Whether the character is currently facing right (or has started turning towards it) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
	FORCEINLINE bool IsFacingRight() const { return Safe_bFacingRight; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
	FORCEINLINE bool IsTurning() const { return Turning; }

	/** To be called by owning character, upon BeginPlay() */
	void SetFacingRight(bool bFacingRight);

	

	/** Checks whether the character is currently in the specified custom movement mode */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
	bool IsCustomMovementMode(EUBCharacterMovementMode Mode) const;
	
	/** Whether the character is wallsliding, or has just left a wall, and can walljump */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
	bool CanWalljump() const;

	/** Begins a 180-degree turn. Returns false if the character is already in a turn */
	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool RequestTurnAround();
	void RequestFaceDirection(float AxisX);

	/** Called when beginning a wallslide. Can be extended via blueprint */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Movement")
	void OnBeginWallslide();

	/** Called when finishing a wallslide. Can be extended via blueprint */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Movement")
	void OnEndWallslide();

	/** Returns true if the character is currently able to dash */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Movement")
	bool CanDash() const;

	/** Called when beginning a dash. Can be extended via blueprint */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Movement")
	void OnBeginDash();

	/** Called when finishing a dash. Can be extended via blueprint */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Movement")
	void OnEndDash();

	/** Returns the last time the owning character dropped through a OneWayPlatform */
	FORCEINLINE float GetLastDropThroughPlatformTime() const { return LastDropThroughPlatformTime; }

	/** Handle Wallsliding replicated from server */
	UFUNCTION()
	virtual void OnRep_IsWallsliding();

	/** Handle Dashing replicated from server */
	UFUNCTION()
	virtual void OnRep_IsDashing();


#pragma endregion

public:
	bool IsMovementMode(EMovementMode InMovementMode) const;

	/// Returns true when we’re in MOVE_Custom + CMOVE_Slide
	UFUNCTION(BlueprintCallable, Category="Buster|Movement")
	bool IsSliding() const
	{
		return MovementMode == MOVE_Custom
			&& CustomMovementMode == UBMOVE_Slide;
	}


#pragma region LedgeGrabVariables
	// LedgeGrab
	//How far away can you LedgeGrab from
	UPROPERTY(EditDefaultsOnly)
	float MaxLedgeGrabDistance = 200;
	
	// How high can you reach to LedgeGrab
	UPROPERTY(EditDefaultsOnly)
	float LedgeGrabReachHeight = 50;
	
	UPROPERTY(EditDefaultsOnly)
	float MinLedgeGrabDepth = 30;
	
	UPROPERTY(EditDefaultsOnly)
	float LedgeGrabMinWallSteepnessAngle = 75;
	
	UPROPERTY(EditDefaultsOnly)
	float LedgeGrabMaxSurfaceAngle = 40;
	
	UPROPERTY(EditDefaultsOnly)
	float LedgeGrabMaxAlignmentAngle = 45;

	//Transition montages enter the main movement
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* MatleMontage;
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* TallLedgeGrabMontage;
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* TransitionTallLedgeGrabMontage;
	UPROPERTY(EditDefaultsOnly)
	float MantleSpeed = 2.f;
	
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* ProxyShortLedgeGrabMontage;
	
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* ProxyTallLedgeGrabMontage;

	UPROPERTY(EditDefaultsOnly)
	float LedgeGrabZOffset = 40.f;

	UPROPERTY(EditDefaultsOnly)
	float LedgeGrabXOffset = -10.f;

	float LedgeGrabSpeed = 110.f;
	float LedgeBrakingDeceleration = 10000.f;
	
	// LedgeGrab 
	bool TryLedgeGrab();
	void PressedGrabLedge();
	bool TryMantle();
	void PhysHang(float deltaTime, int32 Iterations);

	FVector GetLedgeGrabStartLocation(FHitResult FrontHit, FHitResult SurfaceHit);
	//Custom To Be Replaced


	void StopHanging();
	
	UFUNCTION(BlueprintPure) bool IsHanging() const { return IsCustomMovementMode(UBMOVE_Hang); }


private:
	//Helpers
	bool IsServer() const;
	float CapR() const;
	float CapHH() const;

	UFUNCTION()
	void OnRep_LedgeGrab();
# pragma endregion LedgeGrabVariables
private:
	bool bAimingActive = false;
	bool bAimWantsRight = true;

public:
	void SetAimInput(const FVector2D& Aim2D, float Deadzone = 0.10f);
	void ApplyAimFacing(); // call each frame after SetAimInput or directly from Look()
	bool ShouldFreeMoveFacing() const; // walking/crouching/falling and not wallslide/dash/turning
};


