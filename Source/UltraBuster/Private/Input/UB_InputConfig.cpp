// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/UB_InputConfig.h"

const UInputAction* UUB_InputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	// Iterate through the NativeInputActions array
	for (const FBusterInputAction& Action : NativeInputActions)
	{
		// Check if the action is valid and its tag matches the one we're looking for
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction; 
		}
	}
	
	return nullptr; 
}

const UInputAction* UUB_InputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	// Iterate through the AbilityInputActions array
	for (const FBusterInputAction& Action : AbilityInputActions)
	{
		// Check if the action is valid and its tag matches the one we're looking for
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	return nullptr;
}