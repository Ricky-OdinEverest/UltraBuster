// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UB_GameplayAbility.h"
#include "UB_DamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class ULTRABUSTER_API UUB_DamageGameplayAbility : public UUB_GameplayAbility
{
	GENERATED_BODY()

public:
 
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);
protected:
	/**
	 * 
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
 
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;
};
