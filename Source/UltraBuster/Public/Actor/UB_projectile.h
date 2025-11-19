// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "UB_projectile.generated.h"

class UNiagaraSystem;
class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class ULTRABUSTER_API AUB_projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUB_projectile();
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;
 
protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
private:
 
	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 15.f;
 
	bool bHit = false;
 	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;
 
	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;
 
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;
 
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound;
 
	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;

};
