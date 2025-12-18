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

	ULTRABUSTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Reload)
};


struct FUB_GameplayTags
{
public:
	static const FUB_GameplayTags& Get() { return GameplayTags;}
	static void InitializeNativeGameplayTags();
	
	FGameplayTagContainer UB_TagsContainer = FGameplayTagContainer();
	
	FGameplayTag Attributes_MaxHealth;
	FGameplayTag Attributes_Health;

	FGameplayTag Damage;
	FGameplayTag Damage_Bullet;
	FGameplayTag Damage_Fire;
	FGameplayTag Damage_Lightning;
	FGameplayTag Damage_Arcane;
	FGameplayTag Damage_Physical;

	FGameplayTag Reload_Cooldown;

	FGameplayTag Event_Weapon_ReloadStart;

	FGameplayTag Data_RechargeTimestamp;

	
	
	
private:
	static FUB_GameplayTags GameplayTags;
};