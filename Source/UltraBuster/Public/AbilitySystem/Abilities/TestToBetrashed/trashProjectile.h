// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UB_DamageGameplayAbility.h"
#include "trashProjectile.generated.h"
class UGameplayEffect;
class AUB_projectile;

class Atesteffectactor;
/**
 * 
 */
UCLASS()
class ULTRABUSTER_API UtrashProjectile : public UUB_DamageGameplayAbility
{
	GENERATED_BODY()
	
protected:
 
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	// Change the parameter to TSubclassOf
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectile(const FTransform& SpawnTransform, TSubclassOf<Atesteffectactor> ProjectileClass);
};
