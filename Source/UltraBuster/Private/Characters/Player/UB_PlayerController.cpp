// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/UB_PlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/UB_AbilitySystemComponent.h"
#include "Characters/UB_Character.h"
#include "Characters/Player/UB_PlayerState.h"

void AUB_PlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{

	if (UUB_AbilitySystemComponent* ASC = GetBusterAbilitySystemComponent())
	{
		ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}
UUB_AbilitySystemComponent* AUB_PlayerController::GetBusterAbilitySystemComponent()
{

	if(UB_AbilitySystemComponent == nullptr)
	{
		UB_AbilitySystemComponent = Cast<UUB_AbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
		
	}
	return UB_AbilitySystemComponent;
	/*const AUB_Character* OwnerCharacter = Cast<AUB_Character>(GetOwner());
	return (OwnerCharacter ? OwnerCharacter->GetAbilitySystemComponent() : nullptr);*/
}

AUB_PlayerState* AUB_PlayerController::GetBusterPlayerState() const
{
	return CastChecked<AUB_PlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}