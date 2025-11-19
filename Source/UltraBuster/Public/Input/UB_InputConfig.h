// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "UB_InputConfig.generated.h"

class UInputAction;
class UObject;
struct FFrame;
class UInputMappingContext;
/**
 * FLyraInputAction
 *
 *	Struct used to map a input action to a gameplay input tag.
 */
USTRUCT(BlueprintType)
struct FBusterInputAction
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;

	bool IsValid() const
	{
		return InputTag.IsValid() && InputAction;
	}
};
/**
 * UUB_InputConfig
 *
 *	Non-mutable data asset that contains input configuration properties.
 */
UCLASS(BlueprintType, Const)
class ULTRABUSTER_API  UUB_InputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	/**
	 * Finds a UInputAction associated with a given InputTag from the AbilityInputActions list.
	 */
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputMappingContext* DefaultMappingContext;
	// List of input actions used by the owner. These input actions are mapped to
	// a gameplay tag and must be manually bound (e.g., in the PlayerController or Pawn).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FBusterInputAction> NativeInputActions; // NOTE: Using your FBusterInputAction struct

	// List of input actions used by the owner. These input actions are mapped to
	// a gameplay tag and are automatically bound to abilities with matching input tags
	// (if using the Gameplay Ability System).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FBusterInputAction> AbilityInputActions; // NOTE: Using your FBusterInputAction struct
};