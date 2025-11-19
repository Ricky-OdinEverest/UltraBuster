// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UB_DamageGameplayAbility.h"
#include "UB_ProjectileGameplayAbility.generated.h"

class AUB_projectile;
class UGameplayEffect;

/**
 * 
 */
UCLASS()
class ULTRABUSTER_API UUB_ProjectileGameplayAbility : public UUB_DamageGameplayAbility
{
	GENERATED_BODY()
protected:
 
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& MontageTag);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AUB_projectile> ProjectileClass;
};
