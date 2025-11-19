// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"
#include "GameplayTagContainer.h"
/**
 * 
 */
namespace UB_GameplayTags
{

	/** Input Tags **/
	ULTRABUSTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Shoot)
	ULTRABUSTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Melee)
	ULTRABUSTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Dash);
};


struct FUB_GameplayTags
{
public:
	static const FUB_GameplayTags& Get() { return GameplayTags;}
	static void InitializeNativeGameplayTags();

	FGameplayTag Attributes_MaxHealth;
	FGameplayTag Attributes_Health;
	
private:
	static FUB_GameplayTags GameplayTags;
};