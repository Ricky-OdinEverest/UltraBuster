// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/UB_BaseCharacter.h"
#include "UB_BaseEnemy.generated.h"

class AAIController;

//should shift to widget controller
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);

/**
 * 
 */
UCLASS()
class ULTRABUSTER_API AUB_BaseEnemy : public AUB_BaseCharacter
{
	GENERATED_BODY()
public:
	AUB_BaseEnemy(const FObjectInitializer& ObjectInitializer);

	virtual void Die() override;
protected:
	virtual void BeginPlay() override;

	virtual void InitAbilityActorInfo() override;

	virtual void InitializeDefaultAttributes() const override;

	virtual void PossessedBy(AController* NewController) override;
	
	/*UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;
 
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;*/
	


	UPROPERTY()
	TObjectPtr<AAIController> AIController;
	
};
