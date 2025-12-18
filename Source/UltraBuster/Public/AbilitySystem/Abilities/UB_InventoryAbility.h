// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UB_GameplayAbility.h"
#include "UB_InventoryAbility.generated.h"

class UInventoryComponent;
class UInventoryItemInstance;
class UWeaponStaticData;
class AItemActor;

/**
 * 
 */
UCLASS()
class ULTRABUSTER_API UUB_InventoryAbility : public UUB_GameplayAbility
{
	GENERATED_BODY()

public:

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

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

	UPROPERTY()
	UInventoryComponent* InventoryComponent = nullptr;

};
