// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "UB_AbilitySystemComponent.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/);
/**
 * 
 */
UCLASS()
class ULTRABUSTER_API UUB_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	UUB_AbilitySystemComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	//~ UAbilitySystemComponent interface
	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;
	//~ End UAbilitySystemComponent interface

	/** Handles processing input for held abilities */
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);

	/** Clears all ability input */
	void ClearAbilityInput();
	
	/** Process all ability inputs */
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);


	void AbilityActorInfoSet();
	FEffectAssetTags EffectAssetTags;
	
protected:
	virtual void BeginPlay() override;
	


	/**
	 * Handles to abilities that had their input pressed this frame.
	 * Cleared each frame in ProcessAbilityInput.
	 */
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	/**
	 * Handles to abilities that had their input released this frame.
	 * Cleared each frame in ProcessAbilityInput.
	 */
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	/**
	 * Handles to abilities that have their input held.
	 * Cleared when input is released in AbilityInputTagReleased.
	 */
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;



	void EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);
	
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
	                                        const FGameplayEffectSpec& EffectSpec,
	                                        FActiveGameplayEffectHandle ActiveEffectHandle);
};