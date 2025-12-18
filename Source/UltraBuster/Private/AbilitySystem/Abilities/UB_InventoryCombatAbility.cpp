// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/UB_InventoryCombatAbility.h"
#include "Characters/UB_BaseCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "BusterTypes.h"
#include "ActorComponents/InventoryComponent.h"
#include "Kismet/KismetSystemLibrary.h"

bool UUB_InventoryCombatAbility::CommitAbility(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                               FGameplayTagContainer* OptionalRelevantTags)
{
	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags) && HasEnoughAmmo();
}

FGameplayEffectSpecHandle UUB_InventoryCombatAbility::GetWeaponEffectSpec(const FHitResult& InHitResult)
{
	if (UAbilitySystemComponent* AbilityComponent = GetAbilitySystemComponentFromActorInfo())
	{
		if (const UWeaponStaticData* WeaponStaticData = GetEquippedItemWeaponStaticData())
		{
			FGameplayEffectContextHandle EffectContext = AbilityComponent->MakeEffectContext();

			FGameplayEffectSpecHandle OutSpec = AbilityComponent->MakeOutgoingSpec(WeaponStaticData->DamageEffect, 1, EffectContext);

			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(OutSpec, FGameplayTag::RequestGameplayTag(TEXT("Attribute.Health")), -WeaponStaticData->BaseDamage);

			return OutSpec;
		}
	}

	return FGameplayEffectSpecHandle();
}

const bool UUB_InventoryCombatAbility::GetWeaponToFocusTraceResult(float TraceDistance, ETraceTypeQuery TraceType,
	FHitResult& OutHitResult)
{
	AWeaponItemActor* WeaponItemActor = GetEquippedWeaponItemActor();

	AUB_Character* ActionGameCharacter = GetUltraBusterCharacterFromActorInfo();

	//const FTransform& CameraTransform = ActionGameCharacter->GetFollowCamera()->GetComponentTransform();

	//const FVector FocusTraceEnd = CameraTransform.GetLocation() + CameraTransform.GetRotation().Vector() * TraceDistance;

	//TArray<AActor*> ActorsToIgnore = {GetAvatarActorFromActorInfo()};

	

	return OutHitResult.bBlockingHit;
}
bool UUB_InventoryCombatAbility::HasEnoughAmmo() const
{
	if (UAbilitySystemComponent* AbilityComponent = GetAbilitySystemComponentFromActorInfo())
	{
		if (const UWeaponStaticData* WeaponStaticData = GetEquippedItemWeaponStaticData())
		{
			if (UInventoryComponent* Inventory = GetInventoryComponent())
			{
				return !WeaponStaticData->AmmoTag.IsValid() || Inventory->GetInventoryTagCount(WeaponStaticData->AmmoTag) > 0;
			}
		}
	}

	return false;
}
void UUB_InventoryCombatAbility::DecAmmo()
{
	if (UAbilitySystemComponent* AbilityComponent = GetAbilitySystemComponentFromActorInfo())
	{
		if (const UWeaponStaticData* WeaponStaticData = GetEquippedItemWeaponStaticData())
		{
			if(!WeaponStaticData->AmmoTag.IsValid()) return;

			if (UInventoryComponent* Inventory = GetInventoryComponent())
			{
				Inventory->RemoveItemWithInventoryTag(WeaponStaticData->AmmoTag, 1);
			}
		}
	}
}
