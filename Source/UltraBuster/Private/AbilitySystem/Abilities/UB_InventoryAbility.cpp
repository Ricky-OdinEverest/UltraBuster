// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/UB_InventoryAbility.h"

#include "Actor/InventoryActors/WeaponItemActor.h"
#include "ActorComponents/InventoryComponent.h"
#include "Inventory/InventoryItemInstance.h"

void UUB_InventoryAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	InventoryComponent = ActorInfo->OwnerActor.Get()->FindComponentByClass<UInventoryComponent>();
}

UInventoryComponent* UUB_InventoryAbility::GetInventoryComponent() const
{
	return InventoryComponent;
}

UInventoryItemInstance* UUB_InventoryAbility::GetEquippedItemInstance() const
{
	return InventoryComponent ? InventoryComponent->GetEquippedItem() : nullptr;
}

const UItemStaticData* UUB_InventoryAbility::GetEquippedItemStaticData() const
{
	UInventoryItemInstance* EquippedItem = GetEquippedItemInstance();

	return EquippedItem ? EquippedItem->GetItemStaticData() : nullptr;
}

const UWeaponStaticData* UUB_InventoryAbility::GetEquippedItemWeaponStaticData() const
{
	return Cast<UWeaponStaticData>(GetEquippedItemStaticData());
}

AItemActor* UUB_InventoryAbility::GetEquippedItemActor() const
{
	UInventoryItemInstance* EquippedItem = GetEquippedItemInstance();

	return EquippedItem ? EquippedItem->GetItemActor() : nullptr;
}

AWeaponItemActor* UUB_InventoryAbility::GetEquippedWeaponItemActor() const
{
	return Cast<AWeaponItemActor>(GetEquippedItemActor());
}