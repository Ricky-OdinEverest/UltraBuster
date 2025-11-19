// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/UB_PlayerState.h"

#include "AbilitySystem/Attributes/UB_AttributeSet.h"
#include "Net/UnrealNetwork.h"


AUB_PlayerState::AUB_PlayerState()
{
	SetNetUpdateFrequency(100.f);
	AbilitySystemComponent = CreateDefaultSubobject<UUB_AbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UUB_AttributeSet>("AttributeSet");
}

UUB_AbilitySystemComponent* AUB_PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AUB_PlayerState::OnRep_Level(int32 OldLevel)
{
	
}

void AUB_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AUB_PlayerState, Level);
}
