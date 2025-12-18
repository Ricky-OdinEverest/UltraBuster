// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Tasks/AbilityTask_TargetFomPlayer.h"

#include "AbilitySystemComponent.h"

//GrabThoseBusterSockets
#include "Interaction/CombatInterface.h"

UAbilityTask_TargetFomPlayer* UAbilityTask_TargetFomPlayer::CreateTargetFromPlayer(UGameplayAbility* OwningAbility)
{
	UAbilityTask_TargetFomPlayer* MyObj = NewAbilityTask<UAbilityTask_TargetFomPlayer>(OwningAbility);
	return MyObj;
}

void UAbilityTask_TargetFomPlayer::Activate()
{
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
	{
		SendAimData();
	}
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &UAbilityTask_TargetFomPlayer::OnTargetDataReplicatedCallback);
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);
		if (!bCalledDelegate)
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UAbilityTask_TargetFomPlayer::SendAimData()
{
	
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
/////// change for 2d
///
	// Obtain the AvatarActor 
	AActor* AvatarActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	// Default fallback values in case Interface fails
	FVector TraceStart = FVector::ZeroVector;
	FVector TraceEnd = FVector::ZeroVector;

	//Ensure The Combat Interface is Valid
	bool bInterfaceFound = false;

	if (AvatarActor && AvatarActor->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
	{
		//Call CombatInterface To Retrieve Muzzle and Barrel Of our arm cannon
		FVector MuzzleLoc = ICombatInterface::Execute_GetPrimaryMuzzle(AvatarActor);
		FVector BarrelLoc = ICombatInterface::Execute_GetPrimaryBarrel(AvatarActor);

		// Calculate Direction: Normalized Vector from Barrel to Muzzle
		FVector Direction = (MuzzleLoc - BarrelLoc).GetSafeNormal();

		TraceStart = MuzzleLoc;
		// Multiply by 10000 to ensure the trace reaches the target distance
		TraceEnd = MuzzleLoc + (Direction * 10000.f); 

		bInterfaceFound = true;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, TEXT("PlayerNotFound or Interface Missing - Task Failed"));
		
	}

	FHitResult TraceHit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AvatarActor);

	// Perform the line trace
	GetWorld()->LineTraceSingleByChannel(TraceHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);

	//If no hit. The End of the Trace is sent to the ability
	if (!TraceHit.bBlockingHit)
	{
		TraceHit.Location = TraceEnd;
		TraceHit.ImpactPoint = TraceEnd;
		// We set trace start/end explicitly so the client/server know exactly where the line was
		TraceHit.TraceStart = TraceStart;
		TraceHit.TraceEnd = TraceEnd;
	}

	// Build the target data from the trace hit result
	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = TraceHit;
	DataHandle.Add(Data);
	

	// Replicate the target data to the server
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);

	// Broadcast the valid data if needed
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}

}

void UAbilityTask_TargetFomPlayer::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivationTag)
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}