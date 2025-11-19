// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/UB_GameplayAbility.h"
#include "AbilitySystem/UB_AbilitySystemComponent.h"
#include "NativeGameplayTags.h"
#include "Characters/UB_Character.h"




UUB_GameplayAbility::UUB_GameplayAbility(const FObjectInitializer& ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;

	ActivationPolicy = EBusterAbilityActivationPolicy::OnInputTriggered;
	ActivationGroup = EBusterAbilityActivationGroup::Independent;

	bLogCancelation = false;


}


UUB_AbilitySystemComponent* UUB_GameplayAbility::GetUltraBusterAbilitySystemComponentFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<UUB_AbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get()) : nullptr);
}

APlayerController* UUB_GameplayAbility::GetUltraBusterPlayerControllerFromActorInfo() const
{
	// Cast<ALyraPlayerController>(CurrentActorInfo->PlayerController.Get()) : nullptr);
	return (CurrentActorInfo ? CurrentActorInfo->PlayerController.Get() : nullptr);
}

AController* UUB_GameplayAbility::GetControllerFromActorInfo() const
{
	if (CurrentActorInfo)
	{
		if (AController* PC = CurrentActorInfo->PlayerController.Get())
		{
			return PC;
		}

		// Look for a player controller or pawn in the owner chain.
		AActor* TestActor = CurrentActorInfo->OwnerActor.Get();
		while (TestActor)
		{
			if (AController* C = Cast<AController>(TestActor))
			{
				return C;
			}

			if (APawn* Pawn = Cast<APawn>(TestActor))
			{
				return Pawn->GetController();
			}

			TestActor = TestActor->GetOwner();
		}
	}

	return nullptr;
}

AUB_Character* UUB_GameplayAbility::GetUltraBusterCharacterFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<AUB_Character>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

void UUB_GameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec) const
{
	// Try to activate if activation policy is on spawn.
	if (ActorInfo && !Spec.IsActive() && (ActivationPolicy == EBusterAbilityActivationPolicy::OnSpawn))
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

		// If avatar actor is torn off or about to die, don't try to activate until we get the new one.
		if (ASC && AvatarActor && !AvatarActor->GetTearOff() && (AvatarActor->GetLifeSpan() <= 0.0f))
		{
			const bool bIsLocalExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly);
			const bool bIsServerExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated);

			const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
			const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;

			if (bClientShouldActivate || bServerShouldActivate)
			{
				ASC->TryActivateAbility(Spec.Handle);
			}
		}
	}
}

bool UUB_GameplayAbility::CanChangeActivationGroup(EBusterAbilityActivationGroup NewGroup) const
{
	if (!IsInstantiated() || !IsActive())
	{
		return false;
	}

	if (ActivationGroup == NewGroup)
	{
		return true;
	}

	UUB_AbilitySystemComponent* LyraASC = GetUltraBusterAbilitySystemComponentFromActorInfo();
	check(LyraASC);

	// if ((ActivationGroup != EBusterAbilityActivationGroup::Exclusive_Blocking) && LyraASC->IsActivationGroupBlocked(NewGroup))
	// {
	// 	// This ability can't change groups if it's blocked (unless it is the one doing the blocking).
	// 	return false;
	// }

	if ((NewGroup == EBusterAbilityActivationGroup::Exclusive_Replaceable) && !CanBeCanceled())
	{
		// This ability can't become replaceable if it can't be canceled.
		return false;
	}

	return true;
}

bool UUB_GameplayAbility::ChangeActivationGroup(EBusterAbilityActivationGroup NewGroup)
{
	return false;
}

bool UUB_GameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UUB_GameplayAbility::SetCanBeCanceled(bool bCanBeCanceled)
{
	Super::SetCanBeCanceled(bCanBeCanceled);
}

void UUB_GameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
}

void UUB_GameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
}

void UUB_GameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UUB_GameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UUB_GameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
}

void UUB_GameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
}

FGameplayEffectContextHandle UUB_GameplayAbility::MakeEffectContext(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo) const
{
	return Super::MakeEffectContext(Handle, ActorInfo);
}

void UUB_GameplayAbility::ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec,
	FGameplayAbilitySpec* AbilitySpec) const
{
	Super::ApplyAbilityTagsToGameplayEffectSpec(Spec, AbilitySpec);
}

bool UUB_GameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::DoesAbilitySatisfyTagRequirements(AbilitySystemComponent, SourceTags, TargetTags,
	                                                OptionalRelevantTags);
}

void UUB_GameplayAbility::OnPawnAvatarSet()
{
}

void UUB_GameplayAbility::GetAbilitySource(FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, float& OutSourceLevel,
	const IUBAbilitySourceInterface*& OutAbilitySource, AActor*& OutEffectCauser) const
{
}

