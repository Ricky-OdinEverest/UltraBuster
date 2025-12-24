// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"



#include "UB_BaseCharacter.generated.h"

class UAttributeSet;
class UGameplayEffect;
class UAnimMontage;
class UBuster_StartUpData;
class UAbilitySystemComponent;
class UGameplayAbility;
class UUB_AbilitySystemComponent;
class UNiagaraSystem;
class UWidgetComponent;

class UInventoryComponent;
class FOnAttributeChangedSignature;
// could also just include this from another class

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);

UCLASS(Abstract)
class ULTRABUSTER_API AUB_BaseCharacter : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	
	AUB_BaseCharacter(const FObjectInitializer& ObjectInitializer);


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* UltraBuster;
	
	/** Combat Interface */
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;	
	virtual void Die() override;	
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) override;

	virtual FVector GetPrimaryMuzzle_Implementation() override;
	virtual FVector GetPrimaryBarrel_Implementation() override;
	
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	/** end Combat Interface */
	
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();

	//Used with combat interface to grab projectile Socket
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName PrimaryProjectileMuzzle_SocketName;
	
	//Used with combat interface to grab projectile Socket
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName PrimaryProjectileBarrel_SocketName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	FName AimSocketName = FName("AimOrigin"); // Matches the socket you made on clavicle_r
	

	bool bDead = false;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UUB_AbilitySystemComponent* GetBusterAbilitySystemComponent();
	
	UPROPERTY()
	TObjectPtr<UUB_AbilitySystemComponent> BusterAbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	// Effect Class and initialization functions for effects
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<class UGameplayEffect> DefaultPrimaryAttributes;

	// Effect Class and initialization functions for effects
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<class UGameplayEffect> DefaultVitalAttributes;

	// Effect Class and initialization functions for effects
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Attributes")
	TSubclassOf<class UGameplayEffect> DefaultBulletAttributes;
	
	
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const;
	
	virtual void InitializeDefaultAttributes() const;
	
protected:
	
	virtual void InitAbilityActorInfo();
// possible temp class in favor of shifting to startup data to allocate abilities
	UPROPERTY(EditDefaultsOnly, Category = "Buster|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> AmmoWidget;

public:	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	TSoftObjectPtr<UBuster_StartUpData> CharacterStartUpData;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
	UNiagaraSystem* BloodEffect;
private:


	
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;

public:
	
	virtual void BroadcastInitialValues();
	virtual void BindCallbacksToDependencies();
	virtual void InitHealthBarWidget();
	void InitAmmoWidget();

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;
 
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnAmmoChanged;
 
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxAmmoChanged;

	protected:
	UPROPERTY(EditAnywhere, Replicated)
	UInventoryComponent* InventoryComponent;
};
