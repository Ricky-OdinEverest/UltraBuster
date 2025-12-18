// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "testeffectactor.generated.h"

UCLASS()
class ULTRABUSTER_API Atesteffectactor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	Atesteffectactor();

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

};
