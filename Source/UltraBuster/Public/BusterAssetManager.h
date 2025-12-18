// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "BusterAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class ULTRABUSTER_API UBusterAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	static UBusterAssetManager& Get();

	/** This is the key function that starts the loading process */
	virtual void StartInitialLoading() override;
};