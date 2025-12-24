// Fill out your copyright notice in the Description page of Project Settings.


#include "UB_GameplayTags.h"
#define ADDNATIVETAG(AttributeType, AttributeName, Comment) \
UE_DEFINE_GAMEPLAY_TAG_COMMENT(Attributes_##AttributeType##_##AttributeName, *FString(FString("Attributes.") + #AttributeType + FString(".") + #AttributeName), Comment)



namespace UB_GameplayTags
{
	/** Input Tags **/
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Shoot,"InputTag.Shoot")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Melee,"InputTag.Melee")
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Dash,"InputTag.Dash");
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Reload,"InputTag.Reload");

	/** Player Attributes **/
	ADDNATIVETAG(Player, Health, "Current amount of health remaining")
	ADDNATIVETAG(Player, MaxHealth, "Maximum amount of health obtainable")

	/** Weapon Attributes **/
	ADDNATIVETAG(Weapon, Ammo, "Current ammo in the clip")
	ADDNATIVETAG(Weapon, MaxAmmo, "Maximum ammo capacity")
	ADDNATIVETAG(Weapon, RechargeTime, "Timestamp when the weapon will be ready")
	ADDNATIVETAG(Weapon, RechargeBaseDelay, "Initial delay after a shot")
	ADDNATIVETAG(Weapon, RechargePenalty, "Time added per subsequent shot")
	ADDNATIVETAG(Weapon, RechargeDelayCap, "Maximum time the recharge can be pushed out")
	ADDNATIVETAG(Weapon, FireRate, "Rounds per second or delay between shots")
	ADDNATIVETAG(Weapon, Spread, "Bullet spread variance")

	/** Physics Attributes **/
	ADDNATIVETAG(Bullet, ProjectileSpeed, "Velocity of the projectile")
	ADDNATIVETAG(Bullet, ProjectileGravityScale, "Gravity influence on the projectile")
	ADDNATIVETAG(Bullet, ProjectileSize, "Collision size of the projectile")
	ADDNATIVETAG(Bullet, ProjectileBounces, "Number of times projectile can bounce")
	ADDNATIVETAG(Bullet, KnockbackForce, "Physical force applied on impact")

	/** Meta Attributes **/
	//ADDNATIVETAG(Meta, IncomingDamage, "Temporary attribute for incoming damage calculation")

};


FUB_GameplayTags FUB_GameplayTags::GameplayTags;

void FUB_GameplayTags::InitializeNativeGameplayTags()
{
#define ADDTAGTO_CONTAINER(AttributeType, AttributeName) \
GameplayTags.UB_TagsContainer.AddTag(UB_GameplayTags::Attributes_##AttributeType##_##AttributeName);
	// Player
	ADDTAGTO_CONTAINER(Player, Health)
	ADDTAGTO_CONTAINER(Player, MaxHealth)

	// Weapon
	ADDTAGTO_CONTAINER(Weapon, Ammo)
	ADDTAGTO_CONTAINER(Weapon, MaxAmmo)
	ADDTAGTO_CONTAINER(Weapon, RechargeTime)
	ADDTAGTO_CONTAINER(Weapon, RechargeBaseDelay)
	ADDTAGTO_CONTAINER(Weapon, RechargePenalty)
	ADDTAGTO_CONTAINER(Weapon, RechargeDelayCap)
	ADDTAGTO_CONTAINER(Weapon, FireRate)
	ADDTAGTO_CONTAINER(Weapon, Spread)

	// Bullet
	ADDTAGTO_CONTAINER(Bullet, ProjectileSpeed)
	ADDTAGTO_CONTAINER(Bullet, ProjectileGravityScale)
	ADDTAGTO_CONTAINER(Bullet, ProjectileSize)
	ADDTAGTO_CONTAINER(Bullet, ProjectileBounces)
	ADDTAGTO_CONTAINER(Bullet, KnockbackForce)

	// Meta
	//ADDTAGTO_CONTAINER(Meta, IncomingDamage)
	
	// Undefine the macro to keep the scope clean
	#undef ADDTAGTO_CONTAINER
	/*
	GameplayTags.Attributes_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("Attributes.MaxHealth"),
			FString("Maximum amount of Health obtainable")
			);
			*/

	/*
	* Cooldowns 	
*/

	

	GameplayTags.Data_RechargeTimestamp = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Data.RechargeTimestamp"),
FString("Data.RechargeTimestamp")
);

	GameplayTags.Event_Weapon_ReloadStart = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Event.Weapon.ReloadStart"),
FString("Event_Weapon_ReloadStart")
);

	GameplayTags.Reload_Cooldown = UGameplayTagsManager::Get().AddNativeGameplayTag(
FName("Reload.Cooldown"),
FString("Reload Cooldown")
);

	/*
	* Damage Types
*/
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage"),
	FString("Damage")
	);

	

	GameplayTags.Damage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
		 FName("Damage.Fire"),
		 FString("Fire Damage Type")
		 );
	GameplayTags.Damage_Lightning = UGameplayTagsManager::Get().AddNativeGameplayTag(
		 FName("Damage.Lightning"),
		 FString("Lightning Damage Type")
		 );
	GameplayTags.Damage_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Arcane"),
		FString("Arcane Damage Type")
		);
	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Damage.Physical"),
		FString("Physical Damage Type")
		);
 
};