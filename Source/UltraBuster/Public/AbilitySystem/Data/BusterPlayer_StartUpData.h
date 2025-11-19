// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Data/Buster_StartUpData.h"
#include "BusterTypes.h"
#include "BusterPlayer_StartUpData.generated.h"

/**
 * 
 */
UCLASS()
class ULTRABUSTER_API UBusterPlayer_StartUpData : public UBuster_StartUpData
{
	GENERATED_BODY()

public:
	virtual void GiveToAbilitySystemComponent(UUB_AbilitySystemComponent* InASCToGive,int32 ApplyLevel = 1) override;
 
private:
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData", meta = (TitleProperty = "InputTag"))
	TArray<FBusterPlayerAbilitySet> PlayerStartUpAbilitySets;
};