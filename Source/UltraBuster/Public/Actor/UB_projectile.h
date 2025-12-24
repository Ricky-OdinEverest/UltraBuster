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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> Sphere;
protected:
	UFUNCTION()
	void OnSphereHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
private:
 
	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 15.f;
 
	bool bHit = false;
 	

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> MuzzleFlashEffect;
 
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> MuzzleFlashSound;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;
 
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;
 
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound;
 
	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;

public:
	// Attribute Driven Parameters
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "GAS")
	float InitialSpeed = 1750.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "GAS")
	float GravityScale = 0.5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "GAS")
	int32 MaxBounces = 0;

};
