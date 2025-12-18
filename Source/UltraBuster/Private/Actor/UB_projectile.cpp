// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/UB_projectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
//For projectile collision channel
#include "UltraBuster/UltraBuster.h"

AUB_projectile::AUB_projectile()
{
//was false
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
 
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	//May change for destruction later
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	Sphere->SetCollisionResponseToAllChannels(ECR_Block);


	Sphere->IgnoreActorWhenMoving(GetInstigator(), true);

	
//defualts
	/*Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);*/
 
	 ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	 ProjectileMovement->InitialSpeed = 1750.f;
	 ProjectileMovement->MaxSpeed = 1750.f;
	 ProjectileMovement->ProjectileGravityScale = .5f;

	//makes arc look pretty
	ProjectileMovement->bRotationFollowsVelocity = true;

}


void AUB_projectile::OnSphereHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == GetInstigator()) return;
	if (DamageEffectSpecHandle.Data.IsValid() && DamageEffectSpecHandle.Data.Get()->GetContext().GetEffectCauser() == OtherActor)
	{
		return;
	}
	if (!bHit)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
		if (LoopingSoundComponent) LoopingSoundComponent->Stop();
	}
 
	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
		Destroy();
	}
	else
	{
		bHit = true;
	}
}


void AUB_projectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LifeSpan);

	
	Sphere->IgnoreActorWhenMoving(GetInstigator(), true);
	
	//Sphere->OnComponentBeginOverlap.AddDynamic(this, &AUB_projectile::OnSphereOverlap);
	Sphere->OnComponentHit.AddDynamic(this, &AUB_projectile::OnSphereHit);
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
	
	if (MuzzleFlashEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, MuzzleFlashEffect, GetActorLocation(), GetActorRotation());
	}
	if (!bHit && !HasAuthority())
	{
		if (MuzzleFlashSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ImpactSound is null in Destroyed()"));
		}
        
	


	}

}

void AUB_projectile::Destroyed()
{
	if (!bHit && !HasAuthority())
	{
		if (ImpactSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ImpactSound is null in Destroyed()"));
		}
        
		if (ImpactEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ImpactEffect is null in Destroyed()"));
		}
        
		if (LoopingSoundComponent)
		{
			LoopingSoundComponent->Stop();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("LoopingSoundComponent is null in Destroyed()"));
		}
	}
	Super::Destroyed();
}

void AUB_projectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetInstigator()) return;
	if (DamageEffectSpecHandle.Data.IsValid() && DamageEffectSpecHandle.Data.Get()->GetContext().GetEffectCauser() == OtherActor)
	{
		return;
	}
	if (!bHit)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
		if (LoopingSoundComponent) LoopingSoundComponent->Stop();
	}
 
	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
		Destroy();
	}
	else
	{
		bHit = true;
	}
}

