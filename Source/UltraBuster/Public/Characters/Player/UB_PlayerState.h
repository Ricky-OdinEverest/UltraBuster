// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/UB_AbilitySystemComponent.h"

#include "UB_PlayerState.generated.h"

class UUB_AbilitySystemComponent;



/**
 * 
 */
UCLASS()
class ULTRABUSTER_API AUB_PlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AUB_PlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UUB_AbilitySystemComponent* GetAbilitySystemComponent() const override;

	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

private:

	UPROPERTY(VisibleAnywhere, Category = "Crash|Abilities")
	TObjectPtr<UUB_AbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Level)
	int32 Level = 1;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

protected:
	
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
	
};
