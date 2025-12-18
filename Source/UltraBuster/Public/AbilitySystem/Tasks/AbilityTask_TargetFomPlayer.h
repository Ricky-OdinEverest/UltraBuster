// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_TargetFomPlayer.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerTargetDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);


UCLASS()
class ULTRABUSTER_API UAbilityTask_TargetFomPlayer : public UAbilityTask
{
	GENERATED_BODY()

public:
 
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (DisplayName = "TargetFromPlayer", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UAbilityTask_TargetFomPlayer* CreateTargetFromPlayer(UGameplayAbility* OwningAbility);
 
	UPROPERTY(BlueprintAssignable)
	FPlayerTargetDataSignature ValidData;
 
private:
 
	virtual void Activate() override;
	void SendAimData();

	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);
 
};
