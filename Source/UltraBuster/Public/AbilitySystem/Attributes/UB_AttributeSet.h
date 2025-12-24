// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "UB_AttributeSet.generated.h"


#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()

	FEffectProperties(){}

	FGameplayEffectContextHandle EffectContextHandle;

	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;

	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;

	UPROPERTY()
	AController* SourceController = nullptr;

	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;

	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;

	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;

	UPROPERTY()
	AController* TargetController = nullptr;

	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
};
/**
 * @brief Template alias for a static function pointer type.
 *
 * This 'using' declaration creates a shorthand alias named TStaticFuncPtr.
 * It simplifies the complex syntax required to get the raw function pointer type (FFuncPtr)
 * from within Unreal's TBaseStaticDelegateInstance class.
 *
 * In the TMap 'TagsToAttributes' below, this is used to create a pointer to a static
 * function that takes no arguments and returns an FGameplayAttribute:
 * TStaticFuncPtr<FGameplayAttribute()>
 */
template<class T>
using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;
/**
 * 
 */
UCLASS()
class ULTRABUSTER_API UUB_AttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UUB_AttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributes;
#pragma  region Player Attributes
	// --- VITAL ATTRIBUTES ---
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Player Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UUB_AttributeSet, Health);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Player Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UUB_AttributeSet, MaxHealth);
#pragma endregion Player Attributes

#pragma  region Weapon Attributes
	// --- AMMO ATTRIBUTES ---
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Ammo, Category = "Weapon Attributes")
	FGameplayAttributeData Ammo;
	ATTRIBUTE_ACCESSORS(UUB_AttributeSet, Ammo);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxAmmo, Category = "Weapon Attributes")
	FGameplayAttributeData MaxAmmo;
	ATTRIBUTE_ACCESSORS(UUB_AttributeSet, MaxAmmo);

	// --- RECHARGE ATTRIBUTES ---
	// The timestamp when the weapon will be ready (Server Time Seconds)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RechargeTime, Category = "Weapon Attributes")
	FGameplayAttributeData RechargeTime;
	ATTRIBUTE_ACCESSORS(UUB_AttributeSet, RechargeTime);
	
	// The initial delay after a shot (e.g., 3.0s)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RechargeBaseDelay, Category = "Weapon Attributes")
	FGameplayAttributeData RechargeBaseDelay;
	ATTRIBUTE_ACCESSORS(UUB_AttributeSet, RechargeBaseDelay);

	// The time added per subsequent shot (e.g., 0.5s)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RechargePenalty, Category = "Weapon Attributes")
	FGameplayAttributeData RechargePenalty;
	ATTRIBUTE_ACCESSORS(UUB_AttributeSet, RechargePenalty);

	// The maximum time the recharge can be pushed out to (e.g., 3.0s)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RechargeDelayCap, Category = "Weapon Attributes")
	FGameplayAttributeData RechargeDelayCap;
	ATTRIBUTE_ACCESSORS(UUB_AttributeSet, RechargeDelayCap);

	// --- WEAPON HANDLING ATTRIBUTES ---
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FireRate, Category = "Weapon Attributes")
	FGameplayAttributeData FireRate;
	ATTRIBUTE_ACCESSORS(UUB_AttributeSet, FireRate);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Spread, Category = "Weapon Attributes")
	FGameplayAttributeData Spread;
	ATTRIBUTE_ACCESSORS(UUB_AttributeSet, Spread);

	// --- BULLET PHYSICS ATTRIBUTES ---
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ProjectileSpeed, Category = "Bullet Physics")
	FGameplayAttributeData ProjectileSpeed;
	ATTRIBUTE_ACCESSORS(UUB_AttributeSet, ProjectileSpeed);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ProjectileGravityScale, Category = "Bullet Physics")
	FGameplayAttributeData ProjectileGravityScale;
	ATTRIBUTE_ACCESSORS(UUB_AttributeSet, ProjectileGravityScale);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ProjectileSize, Category = "Bullet Physics")
	FGameplayAttributeData ProjectileSize;
	ATTRIBUTE_ACCESSORS(UUB_AttributeSet, ProjectileSize);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ProjectileBounces, Category = "Bullet Physics")
	FGameplayAttributeData ProjectileBounces;
	ATTRIBUTE_ACCESSORS(UUB_AttributeSet, ProjectileBounces);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_KnockbackForce, Category = "Bullet Physics")
	FGameplayAttributeData KnockbackForce;
	ATTRIBUTE_ACCESSORS(UUB_AttributeSet, KnockbackForce);
	
#pragma endregion Weapon Attributes
	// value set locally by gameplay effects
	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UUB_AttributeSet, IncomingDamage);
	
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;

	UFUNCTION()
	void OnRep_Ammo(const FGameplayAttributeData& OldAmmo) const;

	UFUNCTION()
	void OnRep_MaxAmmo(const FGameplayAttributeData& OldMaxAmmo) const;
	
	UFUNCTION()
	void OnRep_RechargeTime(const FGameplayAttributeData& OldRechargeTime) const;

	UFUNCTION()
	void OnRep_RechargeBaseDelay(const FGameplayAttributeData& OldRechargeBaseDelay) const;

	UFUNCTION()
	void OnRep_RechargePenalty(const FGameplayAttributeData& OldRechargePenalty) const;

	UFUNCTION()
	void OnRep_RechargeDelayCap(const FGameplayAttributeData& OldRechargeDelayCap) const;

	UFUNCTION()
	void OnRep_FireRate(const FGameplayAttributeData& OldFireRate) const;

	UFUNCTION()
	void OnRep_Spread(const FGameplayAttributeData& OldSpread) const;

	UFUNCTION()
	void OnRep_ProjectileSpeed(const FGameplayAttributeData& OldProjectileSpeed) const;

	UFUNCTION()
	void OnRep_ProjectileGravityScale(const FGameplayAttributeData& OldProjectileGravityScale) const;

	UFUNCTION()
	void OnRep_ProjectileSize(const FGameplayAttributeData& OldProjectileSize) const;

	UFUNCTION()
	void OnRep_ProjectileBounces(const FGameplayAttributeData& OldProjectileBounces) const;

	UFUNCTION()
	void OnRep_KnockbackForce(const FGameplayAttributeData& OldKnockbackForce) const;
	
private:

	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
	
	void ShowFloatingText(const FEffectProperties& Props, float Damage, bool bBlockedHit, bool bCriticalHit) const;
	
};
