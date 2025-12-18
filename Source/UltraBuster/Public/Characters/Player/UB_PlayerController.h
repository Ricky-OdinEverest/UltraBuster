// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UB_PlayerState.h"
#include "AbilitySystem/UB_AbilitySystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "UB_PlayerController.generated.h"


class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class ULTRABUSTER_API AUB_PlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	UUB_AbilitySystemComponent* GetBusterAbilitySystemComponent();
	AUB_PlayerState* GetBusterPlayerState() const;
	/*void Input_AbilityInputPressed(FGameplayTag InInputTag);
	void Input_AbilityInputReleased(FGameplayTag InInputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);*/

protected:
	virtual void PostProcessInput(float DeltaTime, bool bGamePaused) override;

private:
	
	UPROPERTY()
	TObjectPtr<UUB_AbilitySystemComponent> UB_AbilitySystemComponent;
};