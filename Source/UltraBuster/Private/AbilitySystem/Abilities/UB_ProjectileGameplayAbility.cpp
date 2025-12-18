// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/UB_ProjectileGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/UB_projectile.h"
#include "UB_GameplayTags.h"
//#include "Components/SphereComponent.h"
#include "BusterTypes.h"
#include "Actor/InventoryActors/WeaponItemActor.h"
#include "ActorComponents/InventoryComponent.h"
#include "Interaction/CombatInterface.h"
#include "Inventory/InventoryItemInstance.h"




bool UUB_ProjectileGameplayAbility::CommitAbility(const FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                  FGameplayTagContainer* OptionalRelevantTags)
{
	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

bool UUB_ProjectileGameplayAbility::HasEnoughAmmo() const
{
	/*if (UAbilitySystemComponent* AbilityComponent = GetAbilitySystemComponentFromActorInfo())
	{
		if (const UWeaponStaticData* WeaponStaticData = GetEquippedItemWeaponStaticData())
		{
			if (UInventoryComponent* Inventory = GetInventoryComponent())
			{
				return !WeaponStaticData->AmmoTag.IsValid() || Inventory->GetInventoryTagCount(WeaponStaticData->AmmoTag) > 0;
			}
		}
	}*/

	return false;
}

void UUB_ProjectileGameplayAbility::DecAmmo()
{
	/*if (UAbilitySystemComponent* AbilityComponent = GetAbilitySystemComponentFromActorInfo())
	{
		if (const UWeaponStaticData* WeaponStaticData = GetEquippedItemWeaponStaticData())
		{
			if(!WeaponStaticData->AmmoTag.IsValid()) return;

			if (UInventoryComponent* Inventory = GetInventoryComponent())
			{
				Inventory->RemoveItemWithInventoryTag(WeaponStaticData->AmmoTag, 1);
			}
		}
	}*/
}



void UUB_ProjectileGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                    const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
}

void UUB_ProjectileGameplayAbility::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;
 
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (CombatInterface)
	{
		
		//Get Muzzle For Spawn Location
        const FVector MuzzleSocketLocation = ICombatInterface::Execute_GetPrimaryMuzzle(GetAvatarActorFromActorInfo());
		//const FVector BarrelSocketLocation = ICombatInterface::Execute_GetPrimaryBarrel(GetAvatarActorFromActorInfo(), BarrelName);

		FRotator Rotation = (ProjectileTargetLocation - MuzzleSocketLocation).Rotation();
		
 
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(MuzzleSocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());
 		
		AUB_projectile* Projectile = GetWorld()->SpawnActorDeferred<AUB_projectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetAvatarActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
 
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(Projectile);
		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(Projectile);
		EffectContextHandle.AddActors(Actors);
		FHitResult HitResult;
		HitResult.Location = ProjectileTargetLocation;
		EffectContextHandle.AddHitResult(HitResult);
 		
		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

		const FUB_GameplayTags GameplayTags = FUB_GameplayTags::Get();

		for (auto& Pair : DamageTypes)
		{
			const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
			UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaledDamage);
		}
		
		Projectile->DamageEffectSpecHandle = SpecHandle;

		/*if (AActor* MyAvatar = GetAvatarActorFromActorInfo())
		{
			// 1. Ignore the Avatar (Player) on the Sphere
			if (Projectile->Sphere)
			{
				Projectile->Sphere->IgnoreActorWhenMoving(MyAvatar, true);
			}
    
			// 2. SAFETY LOOP: Find ANY other component (like the Visual Mesh) and ignore the player on those too.
			TArray<UPrimitiveComponent*> MyComponents;
			Projectile->GetComponents<UPrimitiveComponent>(MyComponents);

			for (UPrimitiveComponent* PrimComp : MyComponents)
			{
				PrimComp->IgnoreActorWhenMoving(MyAvatar, true);
			}
		}*/

		Projectile->FinishSpawning(SpawnTransform);
	}
}