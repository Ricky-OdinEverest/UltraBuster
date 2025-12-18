// Fill out your copyright notice in the Description page of Project Settings.


#include "BusterAssetManager.h"

#include "AbilitySystemGlobals.h"
#include "UB_GameplayTags.h"

UBusterAssetManager& UBusterAssetManager::Get()
{
	check(GEngine);
    
	UBusterAssetManager* UBAssetManager = Cast<UBusterAssetManager>(GEngine->AssetManager);
	return *UBAssetManager;
}

void UBusterAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FUB_GameplayTags::InitializeNativeGameplayTags();
	UAbilitySystemGlobals::Get().InitGlobalData();
}
