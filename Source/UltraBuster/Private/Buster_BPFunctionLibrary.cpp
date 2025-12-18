// Fill out your copyright notice in the Description page of Project Settings.


#include "Buster_BPFunctionLibrary.h"

#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/UB_projectile.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Attributes/UB_AttributeSet.h"

static TAutoConsoleVariable<int32> CVarShowRadialDamage(
	TEXT("ShowRadialDamage"),
	0,
	TEXT("Draws debug info about radial damage")
	TEXT(" 0: off/n")
	TEXT(" 1: on/n"),
	ECVF_Cheat
);

const UItemStaticData* UBuster_BPFunctionLibrary::GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass)
{
	if (IsValid(ItemDataClass))
	{
		return GetDefault<UItemStaticData>(ItemDataClass);
	}

	return nullptr;
}

void UBuster_BPFunctionLibrary::ApplyRadialDamage(UObject* WorldContextObject, AActor* DamageCauser, FVector Location,
	float Radius, float DamageAmount, TArray<TSubclassOf<class UGameplayEffect>> DamageEffects,
	const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes, ETraceTypeQuery TraceType)
{
		TArray<AActor*> OutActors;
	TArray<AActor*> ActorsToIgnore = {DamageCauser};

	UKismetSystemLibrary::SphereOverlapActors(WorldContextObject, Location, Radius, ObjectTypes, nullptr, ActorsToIgnore, OutActors);

	const bool bDebug = static_cast<bool>(CVarShowRadialDamage.GetValueOnAnyThread());

	for (AActor* Actor : OutActors)
	{
		FHitResult HitResult;

		if (UKismetSystemLibrary::LineTraceSingle(WorldContextObject, Location, Actor->GetActorLocation(), TraceType, true, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true))
		{
			AActor* Target = HitResult.GetActor();

			if (Target == Actor)
			{
				bool bWasApplied = false;

				if (UAbilitySystemComponent* AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
				{
					FGameplayEffectContextHandle EffectContext = AbilityComponent->MakeEffectContext();
					EffectContext.AddInstigator(DamageCauser, DamageCauser);

					for (auto Effect : DamageEffects)
					{
						FGameplayEffectSpecHandle SpecHandle = AbilityComponent->MakeOutgoingSpec(Effect, 1, EffectContext);
						if (SpecHandle.IsValid())
						{
							UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, FGameplayTag::RequestGameplayTag(TEXT("Attribute.Health")), -DamageAmount);

							FActiveGameplayEffectHandle ActiveGEHandle = AbilityComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

							if (ActiveGEHandle.WasSuccessfullyApplied())
							{
								bWasApplied = true;
							}
						}
					}
				}

				if (bDebug)
				{
					DrawDebugLine(WorldContextObject->GetWorld(), Location, Actor->GetActorLocation(), bWasApplied ? FColor::Green : FColor::Red, false, 4.f, 0, 1);
					DrawDebugSphere(WorldContextObject->GetWorld(), HitResult.Location, 16, 16, bWasApplied ? FColor::Green : FColor::Red, false, 4.f, 0, 1);
					DrawDebugString(WorldContextObject->GetWorld(), HitResult.Location, *GetNameSafe(Target), nullptr, FColor::White, 0, false, 1.f);
				}
			}
			else
			{

				if (bDebug)
				{
					DrawDebugLine(WorldContextObject->GetWorld(), Location, Actor->GetActorLocation(), FColor::Red, false, 4.f, 0, 1);
					DrawDebugSphere(WorldContextObject->GetWorld(), HitResult.Location, 16, 16,  FColor::Red, false, 4.f, 0, 1);
					DrawDebugString(WorldContextObject->GetWorld(), HitResult.Location, *GetNameSafe(Target), nullptr, FColor::Red, 0, false, 1.f);
				}
			}
		}
		else
		{
			if (bDebug)
			{
				DrawDebugLine(WorldContextObject->GetWorld(), Location, Actor->GetActorLocation(), FColor::Red, false, 4.f, 0, 1);
				DrawDebugSphere(WorldContextObject->GetWorld(), HitResult.Location, 16, 16, FColor::Red, false, 4.f, 0, 1);
				DrawDebugString(WorldContextObject->GetWorld(), HitResult.Location, *GetNameSafe(HitResult.GetActor()), nullptr, FColor::Red, 0, false, 1.f);
			}
		}
	}

	if (bDebug)
	{
		DrawDebugSphere(WorldContextObject->GetWorld(), Location, Radius, 16, FColor::White, false, 4.f, 0, 1.f);
	}
}

AUB_projectile* UBuster_BPFunctionLibrary::LaunchProjectile(UObject* WorldContextObject,
	TSubclassOf<UProjectileStaticData> ProjectileDataClass, FTransform Transform, AActor* Owner, APawn* Instigator)
{
	UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;

	if (World && World->GetNetMode() < ENetMode::NM_Client)
	{
		if (AUB_projectile* Projectile = World->SpawnActorDeferred<AUB_projectile>(AUB_projectile::StaticClass(), Transform, Owner, Instigator, ESpawnActorCollisionHandlingMethod::AlwaysSpawn))
		{
			//Projectile->ProjectileDataClass = ProjectileDataClass;
			//Projectile->FinishSpawning(Transform);

			return Projectile;
		}
	}

	return nullptr;
}

float UBuster_BPFunctionLibrary::CalculateNewRechargeTime(UAbilitySystemComponent* ASC, float CurrentServerTime)
{
	if (!ASC) return CurrentServerTime;
	
    
	bool bFound = false;
	float RechargeTime = ASC->GetGameplayAttributeValue(UUB_AttributeSet::GetRechargeTimeAttribute(), bFound);
	float BaseDelay = ASC->GetGameplayAttributeValue(UUB_AttributeSet::GetRechargeBaseDelayAttribute(), bFound);
	float Penalty = ASC->GetGameplayAttributeValue(UUB_AttributeSet::GetRechargePenaltyAttribute(), bFound);
	float DelayCap = ASC->GetGameplayAttributeValue(UUB_AttributeSet::GetRechargeDelayCapAttribute(), bFound);
	
	float NewTime = 0.f;
    
	if (RechargeTime > CurrentServerTime)
	{
		// Timer is running, add penalty
		float RawTime = RechargeTime + Penalty;
		float HardLimit = CurrentServerTime + DelayCap;
		NewTime = FMath::Min(RawTime, HardLimit);
	}
	else
	{
		// Timer finished, start fresh
		NewTime = CurrentServerTime + BaseDelay;
	}

	return NewTime;
}
