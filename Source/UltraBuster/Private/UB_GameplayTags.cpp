// Fill out your copyright notice in the Description page of Project Settings.


#include "UB_GameplayTags.h"

namespace UB_GameplayTags
{
	/** Input Tags **/
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Shoot,"InputTag.Shoot")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Melee,"InputTag.Melee")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Dash,"InputTag.Dash");
};


FUB_GameplayTags FUB_GameplayTags::GameplayTags;

void FUB_GameplayTags::InitializeNativeGameplayTags()
{
	GameplayTags.Attributes_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.MaxHealth"),
			FString("Maximum amount of Health obtainable")
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