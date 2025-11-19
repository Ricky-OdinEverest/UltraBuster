// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Input/UB_InputConfig.h"

#include "BusterInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class ULTRABUSTER_API UBusterInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template<class UserObject,typename CallbackFunc>
	void BindNativeInputAction(const UUB_InputConfig* InInputConfig,const FGameplayTag& InInputTag,ETriggerEvent TriggerEvent,UserObject* ContextObject,CallbackFunc Func);

	template<class UserObject,typename CallbackFunc>
	void BindAbilityInputAction(const UUB_InputConfig* InInputConfig,UserObject* ContextObject,CallbackFunc InputPressedFunc,CallbackFunc InputRelasedFunc);
	template <class UserClass, class PressedFuncType, class ReleasedFuncType, class HeldFuncType>
	void BindAbilityActions(const UUB_InputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc,
	                        ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc);
};

template<class UserObject, typename CallbackFunc>
inline void UBusterInputComponent::BindNativeInputAction(const UUB_InputConfig* InInputConfig, const FGameplayTag& InInputTag, ETriggerEvent TriggerEvent, UserObject* ContextObject, CallbackFunc Func)
{
	checkf(InInputConfig,TEXT("Input config data asset is null,can not proceed with binding"));

	if (const UInputAction* FoundAction = InInputConfig->FindNativeInputActionForTag(InInputTag))
	{
		BindAction(FoundAction,TriggerEvent,ContextObject,Func);
	}
}

template<class UserObject, typename CallbackFunc>
inline void UBusterInputComponent::BindAbilityInputAction(const UUB_InputConfig* InInputConfig, UserObject* ContextObject, CallbackFunc InputPressedFunc, CallbackFunc InputRelasedFunc)
{
	checkf(InInputConfig,TEXT("Input config data asset is null,can not proceed with binding"));

	for (const FBusterInputAction& AbilityInputActionConfig : InInputConfig->AbilityInputActions)
	{
		if(!AbilityInputActionConfig.IsValid()) continue;


		/*
		if (GEngine)
		{
			const FString ActionName = AbilityInputActionConfig.InputAction ? AbilityInputActionConfig.InputAction->GetName() : TEXT("NONE");
			const FString TagName = AbilityInputActionConfig.InputTag.ToString();
			const FString LogMsg = FString::Printf(TEXT("[InputBinding] Action: %s  ==>  Tag: %s"), *ActionName, *TagName);
           
			
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, LogMsg);
		}
		*/
		

		BindAction(AbilityInputActionConfig.InputAction,ETriggerEvent::Started,ContextObject,InputPressedFunc,AbilityInputActionConfig.InputTag);
		BindAction(AbilityInputActionConfig.InputAction,ETriggerEvent::Completed,ContextObject,InputRelasedFunc,AbilityInputActionConfig.InputTag);
	}
}

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
 inline void UBusterInputComponent::BindAbilityActions(const UUB_InputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);
 
	for (const FBusterInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}
 
			if (ReleasedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
			}
 			
			if (HeldFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Action.InputTag);
			}
		}
	}
}
