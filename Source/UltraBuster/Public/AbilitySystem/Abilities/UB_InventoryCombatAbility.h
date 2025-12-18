// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UB_InventoryAbility.h"
#include "UB_InventoryCombatAbility.generated.h"

/**
 * 
 */
UCLASS()
class ULTRABUSTER_API UUB_InventoryCombatAbility : public UUB_InventoryAbility
{
	GENERATED_BODY()
	
public:

	virtual bool CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags) override;

	UFUNCTION(BlueprintPure)
	FGameplayEffectSpecHandle GetWeaponEffectSpec(const FHitResult& InHitResult);

	UFUNCTION(BlueprintPure)
	const bool GetWeaponToFocusTraceResult(float TraceDistance, ETraceTypeQuery TraceType, FHitResult& OutHitResult);

	UFUNCTION(BlueprintPure)
	bool HasEnoughAmmo() const;

	UFUNCTION(BlueprintCallable)
	void DecAmmo();

};
