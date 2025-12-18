// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UB_ProjectileGameplayAbility.h"
#include "REFAMMOProjectileGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class ULTRABUSTER_API UREFAMMOProjectileGameplayAbility : public UUB_ProjectileGameplayAbility
{
	GENERATED_BODY()
	
};


/*
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UB_DamageGameplayAbility.h"
#include "UB_ProjectileGameplayAbility.generated.h"

class AUB_projectile;
class UGameplayEffect;
class UInventoryComponent;
class UInventoryItemInstance;
class UItemStaticData;

/**
 * 
 #1#
UCLASS()
class ULTRABUSTER_API UUB_ProjectileGameplayAbility : public UUB_DamageGameplayAbility
{
	GENERATED_BODY()
	
public:
	// Assign the Inventory Component Variable When the ability is given
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	// Dealing with ammo by checking before we commit the ability
	virtual bool CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags) override;

	
	UFUNCTION(BlueprintPure)
	bool HasEnoughAmmo() const;

	UFUNCTION(BlueprintCallable)
	void DecAmmo();

	
	UFUNCTION(BlueprintPure)
	UInventoryComponent* GetInventoryComponent() const;

	UFUNCTION(BlueprintPure)
	UInventoryItemInstance* GetEquippedItemInstance() const;

	UFUNCTION(BlueprintPure)
	const UItemStaticData* GetEquippedItemStaticData() const;
	
	UFUNCTION(BlueprintPure)
	const UWeaponStaticData* GetEquippedItemWeaponStaticData() const;
	
	UFUNCTION(BlueprintPure)
	AItemActor* GetEquippedItemActor() const;

	UFUNCTION(BlueprintPure)
	AWeaponItemActor* GetEquippedWeaponItemActor() const;


protected:
 
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;
	
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SpawnProjectile(const FVector& ProjectileTargetLocation);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AUB_projectile> ProjectileClass;
	
// allows us to access ammo and weapon data
	UPROPERTY()
	UInventoryComponent* InventoryComponent = nullptr;
};
*/
