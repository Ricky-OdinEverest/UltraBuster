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
};