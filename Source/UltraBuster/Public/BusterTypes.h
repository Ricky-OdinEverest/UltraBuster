// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GameplayTagContainer.h"
#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/UB_GameplayAbility.h"
#include "BusterTypes.generated.h"


class AItemActor;
class UGameplayAbility;
class UGameplayEffect;
class UAnimMontage;
class UNiagaraSystem;
class UInputMappingContext;



/*
USTRUCT(BlueprintType)
struct FWarriorHeroWeaponData
{
	GENERATED_BODY()
 
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<UWarriorHeroLinkedAnimLayer> WeaponAnimLayerToLink;
	 
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UInputMappingContext* WeaponInputMappingContext;
	 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
	TArray<FWarriorHeroAbilitySet> DefaultWeaponAbilities;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FScalableFloat WeaponBaseDamage;
	
};
*/

USTRUCT(BlueprintType)
struct FBusterPlayerAbilitySet
{
	GENERATED_BODY()
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UUB_GameplayAbility> AbilityToGrant;
 
	bool IsValid() const;
};

UCLASS(BlueprintType, Blueprintable)
class UWeaponModStaticData : public UObject
{

	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName Name;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UInputMappingContext* WeaponInputMappingContext;
	 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
	TArray<FBusterPlayerAbilitySet> DefaultWeaponAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AItemActor> ItemActorClass;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	//FName AttachmentSocket = NAME_None;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	//bool bCanBeEquipped = false;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	//FCharacterAnimationData CharacterAnimationData;

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayEffect>> OngoingEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FGameplayTag> InventoryTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxStackCount = 1;*/
};

UCLASS(BlueprintType, Blueprintable)
class UItemStaticData : public UObject
{

	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AItemActor> ItemActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName AttachmentSocket = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bCanBeEquipped = false;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	//FCharacterAnimationData CharacterAnimationData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayEffect>> OngoingEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FGameplayTag> InventoryTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxStackCount = 1;
};

UCLASS(BlueprintType, Blueprintable)
class UWeaponStaticData : public UItemStaticData
{

	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh* StaticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FireRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* AttackSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AmmoTag;
};

UCLASS(BlueprintType, Blueprintable)
class UAmmoItemStaticData : public UItemStaticData
{

	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh* StaticMesh = nullptr;
};

UENUM(BlueprintType)
enum class EItemState : uint8
{
	None  UMETA(DisplayName = "None"),
	Equipped UMETA(DisplayName = "Equipped"),
	Dropped UMETA(DisplayName = "Dropped"),
};

UCLASS(BlueprintType, Blueprintable)
class UProjectileStaticData : public UObject
{

	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamageRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float GravityMultiplayer = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float InitialSpeed = 3000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxSpeed = 3000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh* StaticMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayEffect>> Effects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TEnumAsByte<EObjectTypeQuery>> RadialDamageQueryTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<ETraceTypeQuery> RadialDamageTraceType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* OnStopVFX = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* OnStopSFX = nullptr;
};

