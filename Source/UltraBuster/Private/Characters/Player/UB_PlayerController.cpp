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

}

AUB_PlayerState* AUB_PlayerController::GetBusterPlayerState() const
{
	return CastChecked<AUB_PlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

/*void AUB_PlayerController::Input_AbilityInputPressed(FGameplayTag InInputTag) 
{
	if (GetBusterAbilitySystemComponent()) GetBusterAbilitySystemComponent()->AbilityInputTagPressed(InInputTag);
}

void AUB_PlayerController::Input_AbilityInputReleased(FGameplayTag InInputTag) 
{
	if (GetBusterAbilitySystemComponent()) GetBusterAbilitySystemComponent()->AbilityInputTagReleased(InInputTag);
}

void AUB_PlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetBusterAbilitySystemComponent()) GetBusterAbilitySystemComponent()->AbilityInputTagHeld(InputTag);
}*/