// Fill out your copyright notice in the Description page of Project Settings.


#include "UB_GameplayTags.h"

namespace UB_GameplayTags
{
	/** Input Tags **/
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Shoot,"InputTag.Shoot")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Melee,"InputTag.Melee")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Dash,"InputTag.Dash");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Reload,"InputTag.Reload");

};


FUB_GameplayTags FUB_GameplayTags::GameplayTags;

void FUB_GameplayTags::InitializeNativeGameplayTags()
{
	GameplayTags.Attributes_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.MaxHealth"),
			FString("Maximum amount of Health obtainable")
			);

	/*
	* Cooldowns 	
*/

	

	GameplayTags.Data_RechargeTimestamp = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Data.RechargeTimestamp"),
FString("Data.RechargeTimestamp")
);

	GameplayTags.Event_Weapon_ReloadStart = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Event.Weapon.ReloadStart"),
FString("Event_Weapon_ReloadStart")
);

	GameplayTags.Reload_Cooldown = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Reload.Cooldown"),
FString("Reload Cooldown")
);

	/*
	* Damage Types
*/
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage"),
	FString("Damage")
	);

	

	GameplayTags.Damage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
		 FName("Damage.Fire"),
		 FString("Fire Damage Type")
		 );
	GameplayTags.Damage_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(
		 FName("Damage.Lightning"),
		 FString("Lightning Damage Type")
		 );
	GameplayTags.Damage_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Arcane"),
		FString("Arcane Damage Type")
		);
	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Physical"),
		FString("Physical Damage Type")
		);
 
};