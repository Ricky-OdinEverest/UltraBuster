// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/BusterPlayer_StartUpData.h"
#include "AbilitySystem/Abilities/UB_GameplayAbility.h"
#include "AbilitySystem/UB_AbilitySystemComponent.h"

void UBusterPlayer_StartUpData::GiveToAbilitySystemComponent(UUB_AbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);
 
	for (const FBusterPlayerAbilitySet& AbilitySet : PlayerStartUpAbilitySets)
	{
		if(!AbilitySet.IsValid()) continue;
 
		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);

		/*if (GEngine)
		{
			const FString AbilityName = AbilitySet.AbilityToGrant ? AbilitySet.AbilityToGrant->GetName() : TEXT("NONE");
			const FString TagName = AbilitySet.InputTag.ToString();
			const FString LogMsg = FString::Printf(TEXT("[AbilityGrant] Ability: %s  <==  Tag: %s"), *AbilityName, *TagName);
           
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, LogMsg);
		}*/
 
		InASCToGive->GiveAbility(AbilitySpec);
	}
}