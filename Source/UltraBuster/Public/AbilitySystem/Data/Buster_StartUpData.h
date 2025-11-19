// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Buster_StartUpData.generated.h"

class UUB_GameplayAbility;
class UUB_AbilitySystemComponent;
class UGameplayEffect;
/**
/**
 * 
 */
UCLASS()
class ULTRABUSTER_API UBuster_StartUpData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	virtual void GiveToAbilitySystemComponent(UUB_AbilitySystemComponent* InASCToGive,int32 ApplyLevel = 1);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray< TSubclassOf < UUB_GameplayAbility > > ActivateOnGivenAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray< TSubclassOf < UUB_GameplayAbility > > ReactiveAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
	TArray< TSubclassOf < UGameplayEffect > > StartUpGameplayEffects;

	void GrantAbilities(const TArray< TSubclassOf < UUB_GameplayAbility > >& InAbilitiesToGive,UUB_AbilitySystemComponent* InASCToGive,int32 ApplyLevel = 1);
};