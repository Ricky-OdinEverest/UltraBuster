// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BusterTypes.h"
#include "Buster_BPFunctionLibrary.generated.h"

class AUB_projectile;
/**
 * 
 */
UCLASS()
class ULTRABUSTER_API UBuster_BPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const UItemStaticData* GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void ApplyRadialDamage(UObject* WorldContextObject, AActor* DamageCauser, FVector Location, float Radius, float DamageAmount, TArray<TSubclassOf<class UGameplayEffect>> DamageEffects,
	const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes, ETraceTypeQuery TraceType);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static AUB_projectile* LaunchProjectile(UObject* WorldContextObject, TSubclassOf<UProjectileStaticData> ProjectileDataClass, FTransform Transform, AActor* Owner, APawn* Instigator);

	UFUNCTION(BlueprintPure, Category = "UltraBuster|Combat")
	static float CalculateNewRechargeTime(UAbilitySystemComponent* ASC, float CurrentServerTime);
};
