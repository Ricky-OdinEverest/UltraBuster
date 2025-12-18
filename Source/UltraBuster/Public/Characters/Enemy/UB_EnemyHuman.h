// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Enemy/UB_BaseEnemy.h"
#include "UB_EnemyHuman.generated.h"

/**
 * 
 */
UCLASS()
class ULTRABUSTER_API AUB_EnemyHuman : public AUB_BaseEnemy
{
	GENERATED_BODY()
public:
	AUB_EnemyHuman(const FObjectInitializer& ObjectInitializer);
protected:

	//~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface

	virtual void Die() override;

private:
	void InitEnemyStartUpData() const;
 
	
};
