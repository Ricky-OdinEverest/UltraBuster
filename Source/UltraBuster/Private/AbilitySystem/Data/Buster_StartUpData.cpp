// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/Buster_StartUpData.h"
#include "AbilitySystem/UB_AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/UB_GameplayAbility.h" 

void UBuster_StartUpData::GiveToAbilitySystemComponent(UUB_AbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	check(InASCToGive);

	GrantAbilities(ActivateOnGivenAbilities,InASCToGive,ApplyLevel);
	GrantAbilities(ReactiveAbilities,InASCToGive,ApplyLevel);

	if (!StartUpGameplayEffects.IsEmpty())
	{
		for (const TSubclassOf < UGameplayEffect >& EffectClass : StartUpGameplayEffects)
		{
			if(!EffectClass) continue;
 
			UGameplayEffect* EffectCDO = EffectClass->GetDefaultObject<UGameplayEffect>();
 
			InASCToGive->ApplyGameplayEffectToSelf(
				EffectCDO,
				ApplyLevel,
				InASCToGive->MakeEffectContext()
			);
		}
	}
}

void UBuster_StartUpData::GrantAbilities(const TArray<TSubclassOf<UUB_GameplayAbility>>& InAbilitiesToGive, UUB_AbilitySystemComponent* InASCToGive, int32 ApplyLevel)
{
	if (InAbilitiesToGive.IsEmpty())
	{
		return;
	}

	for (const TSubclassOf<UUB_GameplayAbility>& Ability : InAbilitiesToGive)
	{
		if(!Ability) continue;

		FGameplayAbilitySpec AbilitySpec(Ability);
		AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;

		InASCToGive->GiveAbility(AbilitySpec);
	}
}