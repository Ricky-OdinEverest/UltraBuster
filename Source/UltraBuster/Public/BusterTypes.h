// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameplayTagContainer.h"
#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UB_GameplayAbility.h"
#include "BusterTypes.generated.h"


USTRUCT(BlueprintType)
struct FBusterPlayerAbilitySet
{
	GENERATED_BODY()
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUB_GameplayAbility> AbilityToGrant;
 
	bool IsValid() const;
};