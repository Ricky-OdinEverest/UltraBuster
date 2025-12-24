// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/UB_AttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "UB_GameplayTags.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Net/UnrealNetwork.h"

UUB_AttributeSet::UUB_AttributeSet()
{
	// Define the macro locally for cleanliness
	// Note: We access the tags via the namespace UB_GameplayTags::
#define MAPTAGSTOATTRIBUTES(AttributeType, AttributeName) \
TagsToAttributes.Add(UB_GameplayTags::Attributes_##AttributeType##_##AttributeName, Get##AttributeName##Attribute);

	/*
	 * Player Attributes
	 */
	MAPTAGSTOATTRIBUTES(Player, Health)
	MAPTAGSTOATTRIBUTES(Player, MaxHealth)

	/*
	 * Weapon Attributes
	 */
	MAPTAGSTOATTRIBUTES(Weapon, Ammo)
	MAPTAGSTOATTRIBUTES(Weapon, MaxAmmo)
	MAPTAGSTOATTRIBUTES(Weapon, RechargeTime)
	MAPTAGSTOATTRIBUTES(Weapon, RechargeBaseDelay)
	MAPTAGSTOATTRIBUTES(Weapon, RechargePenalty)
	MAPTAGSTOATTRIBUTES(Weapon, RechargeDelayCap)
	MAPTAGSTOATTRIBUTES(Weapon, FireRate)
	MAPTAGSTOATTRIBUTES(Weapon, Spread)

	/*
	 * Bullet Attributes
	 */
	MAPTAGSTOATTRIBUTES(Bullet, ProjectileSpeed)
	MAPTAGSTOATTRIBUTES(Bullet, ProjectileGravityScale)
	MAPTAGSTOATTRIBUTES(Bullet, ProjectileSize)
	MAPTAGSTOATTRIBUTES(Bullet, ProjectileBounces)
	MAPTAGSTOATTRIBUTES(Bullet, KnockbackForce)

	/*
	 * Meta Attributes
	 */
	//MAPTAGSTOATTRIBUTES(Meta, IncomingDamage)

	// Clean up macro
#undef MAPTAGSTOATTRIBUTES
}

void UUB_AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}

	if (Attribute == GetAmmoAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxAmmo());
	}

	
}

void UUB_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	FEffectProperties Props;
	SetEffectProperties(Data, Props);

	//if(Props.TargetCharacter->Implements<UCombatInterface>() && ICombatInterface::Execute_IsDead(Props.TargetCharacter)) return;

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}

	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalIncomingDamage = GetIncomingDamage();
		//cache local value then set server val to 0 before calculating
		SetIncomingDamage(0.f);
		if (LocalIncomingDamage > 0.f)
		{
			const float NewHealth = GetHealth() - LocalIncomingDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

			const bool bFatal = NewHealth <= 0.f;

			if (bFatal)
			{
				ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor);
				if (CombatInterface)
				{
					CombatInterface->Die();
				}
				// Only used for restricting AI movement at the moment
				//USCR_MeleeBPFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(),SCR_GameplayTags::Shared_Status_Dead);
			}
		}

		
	}

}

void UUB_AttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	// Source = causer of the effect, Target = target of the effect (owner of this AS)

	// Store the GameplayEffectContext from the incoming EffectSpec (contains all contextual info about the effect).
	Props.EffectContextHandle = Data.EffectSpec.GetContext();

	// Get the source (instigator's) AbilitySystemComponent from the context.
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	// Validate that the SourceASC exists and that the associated ActorInfo and AvatarActor are valid.
	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		// Get the source's avatar actor (e.g., character or pawn who caused the effect).
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();

		// Get the PlayerController associated with the source (if available).
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();

		// If no controller was directly found, try to get it from the avatar (e.g., from a pawn).
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			// Cast the avatar to APawn and try to get its controller.
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}

		// If we successfully found a controller, get the pawn it is controlling and cast it to a character.
		Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
	}

	// Now gather info about the target of the effect (usually the actor whose AttributeSet is being modified).
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		// Get the target's avatar actor (e.g., the player or NPC receiving the effect).
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();

		// Get the controller associated with the target.
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();

		// Attempt to cast the avatar actor to an ACharacter for further use (e.g., animations, hit reactions).
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);

		// Retrieve the AbilitySystemComponent from the target's avatar actor using the blueprint library helper.
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}


void UUB_AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// *** VITAL ATTRIBUTES ***
	DOREPLIFETIME_CONDITION_NOTIFY(UUB_AttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UUB_AttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	
	// *** AMMO ATTRIBUTES ***
	DOREPLIFETIME_CONDITION_NOTIFY(UUB_AttributeSet, Ammo, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UUB_AttributeSet, MaxAmmo, COND_None, REPNOTIFY_Always);

	// *** RECHARGE ATTRIBUTES ***
	DOREPLIFETIME_CONDITION_NOTIFY(UUB_AttributeSet, RechargeTime, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UUB_AttributeSet, RechargeBaseDelay, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UUB_AttributeSet, RechargePenalty, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UUB_AttributeSet, RechargeDelayCap, COND_None, REPNOTIFY_Always);

	// *** WEAPON ATTRIBUTES ***
	DOREPLIFETIME_CONDITION_NOTIFY(UUB_AttributeSet, FireRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UUB_AttributeSet, Spread, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UUB_AttributeSet, ProjectileSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UUB_AttributeSet, ProjectileGravityScale, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UUB_AttributeSet, ProjectileSize, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UUB_AttributeSet, ProjectileBounces, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UUB_AttributeSet, KnockbackForce, COND_None, REPNOTIFY_Always);
}

void UUB_AttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUB_AttributeSet, Health, OldHealth);
}

void UUB_AttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUB_AttributeSet, MaxHealth, OldMaxHealth);
}

void UUB_AttributeSet::OnRep_Ammo(const FGameplayAttributeData& OldAmmo) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUB_AttributeSet, Ammo, OldAmmo);

}

void UUB_AttributeSet::OnRep_MaxAmmo(const FGameplayAttributeData& OldMaxAmmo) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUB_AttributeSet, MaxAmmo, OldMaxAmmo);

}

void UUB_AttributeSet::OnRep_RechargeTime(const FGameplayAttributeData& OldRechargeTime) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUB_AttributeSet, RechargeTime, OldRechargeTime);
}

void UUB_AttributeSet::OnRep_RechargeBaseDelay(const FGameplayAttributeData& OldRechargeBaseDelay) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUB_AttributeSet, RechargeBaseDelay, OldRechargeBaseDelay);
}

void UUB_AttributeSet::OnRep_RechargePenalty(const FGameplayAttributeData& OldRechargePenalty) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUB_AttributeSet, RechargePenalty, OldRechargePenalty);
}

void UUB_AttributeSet::OnRep_RechargeDelayCap(const FGameplayAttributeData& OldRechargeDelayCap) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUB_AttributeSet, RechargeDelayCap, OldRechargeDelayCap);
}

void UUB_AttributeSet::OnRep_FireRate(const FGameplayAttributeData& OldFireRate) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUB_AttributeSet, FireRate, OldFireRate);
}

void UUB_AttributeSet::OnRep_Spread(const FGameplayAttributeData& OldSpread) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUB_AttributeSet, Spread, OldSpread);
}

void UUB_AttributeSet::OnRep_ProjectileSpeed(const FGameplayAttributeData& OldProjectileSpeed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUB_AttributeSet, ProjectileSpeed, OldProjectileSpeed);
}

void UUB_AttributeSet::OnRep_ProjectileGravityScale(const FGameplayAttributeData& OldProjectileGravityScale) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUB_AttributeSet, ProjectileGravityScale, OldProjectileGravityScale);
}

void UUB_AttributeSet::OnRep_ProjectileSize(const FGameplayAttributeData& OldProjectileSize) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUB_AttributeSet, ProjectileSize, OldProjectileSize);
}

void UUB_AttributeSet::OnRep_ProjectileBounces(const FGameplayAttributeData& OldProjectileBounces) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUB_AttributeSet, ProjectileBounces, OldProjectileBounces);
}

void UUB_AttributeSet::OnRep_KnockbackForce(const FGameplayAttributeData& OldKnockbackForce) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUB_AttributeSet, KnockbackForce, OldKnockbackForce);
}


void UUB_AttributeSet::ShowFloatingText(const FEffectProperties& Props, float Damage, bool bBlockedHit,
	bool bCriticalHit) const
{
}
