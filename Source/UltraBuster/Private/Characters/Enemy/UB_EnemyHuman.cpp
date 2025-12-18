// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Enemy/UB_EnemyHuman.h"

#include "AbilitySystem/Data/Buster_StartUpData.h"
#include "Engine/AssetManager.h"

AUB_EnemyHuman::AUB_EnemyHuman(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AUB_EnemyHuman::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitEnemyStartUpData();
}

void AUB_EnemyHuman::Die()
{
	Super::Die();
}

void AUB_EnemyHuman::InitEnemyStartUpData() const
{
	if (CharacterStartUpData.IsNull())
	{
		return;
	}
 
	UAssetManager::GetStreamableManager().RequestAsyncLoad(
		CharacterStartUpData.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
			[this]()
			{
				if (UBuster_StartUpData* LoadedData = CharacterStartUpData.Get())
				{
					LoadedData->GiveToAbilitySystemComponent(static_cast<UUB_AbilitySystemComponent*>(AbilitySystemComponent));


				}
			}
		)
		);
}
