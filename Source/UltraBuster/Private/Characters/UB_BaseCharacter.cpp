// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/UB_BaseCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/UB_AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/UB_AttributeSet.h"
#include "ActorComponents/InventoryComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "UI/Widget/Buster_UserWidget.h"
#include "UltraBuster/UltraBuster.h"


struct FInputActionValue;

AUB_BaseCharacter::AUB_BaseCharacter(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	UltraBuster = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ultra Buster"));
	UltraBuster->SetupAttachment(GetMesh(), FName("Buster_r"));
	
	// --- HEALTH WIDGET SETUP ---
	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());

	// --- AMMO WIDGET SETUP ---
	AmmoWidget = CreateDefaultSubobject<UWidgetComponent>("AmmoWidget");
	AmmoWidget->SetupAttachment(UltraBuster, FName("AmmoHudSocket")); 
	AmmoWidget->SetWidgetSpace(EWidgetSpace::World);
	AmmoWidget->SetDrawAtDesiredSize(true);
	AmmoWidget->SetTwoSided(true);
	// --- InventoryComponent (UNUSED) SETUP ---
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory Component"));
	InventoryComponent->SetIsReplicated(true);
}

UAnimMontage* AUB_BaseCharacter::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void AUB_BaseCharacter::Die()
{
	MulticastHandleDeath();
}

void AUB_BaseCharacter::MulticastHandleDeath_Implementation()
{
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
 	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Dissolve();
	bDead = true;
}


FVector AUB_BaseCharacter::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag)
{
	return GetMesh()->GetSocketLocation(PrimaryProjectileBarrel_SocketName); 
}

FVector AUB_BaseCharacter::GetPrimaryMuzzle_Implementation()
{
	return UltraBuster->GetSocketLocation(PrimaryProjectileMuzzle_SocketName);
}

FVector AUB_BaseCharacter::GetPrimaryBarrel_Implementation()
{
	return UltraBuster->GetSocketLocation(PrimaryProjectileBarrel_SocketName);
}

bool AUB_BaseCharacter::IsDead_Implementation() const
{
	return bDead;
}

AActor* AUB_BaseCharacter::GetAvatar_Implementation()
{
	return this;
}

UNiagaraSystem* AUB_BaseCharacter::GetBloodEffect_Implementation()
{
	return BloodEffect;
}


UAbilitySystemComponent* AUB_BaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AUB_BaseCharacter::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}


void AUB_BaseCharacter::InitializeDefaultAttributes() const
{
	//Have to be applied in order to ensure values exist
	// IE Max Health Before Health
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

void AUB_BaseCharacter::InitAbilityActorInfo()
{
}

UUB_AbilitySystemComponent* AUB_BaseCharacter::GetBusterAbilitySystemComponent()
{
	if(BusterAbilitySystemComponent == nullptr)
	{
		BusterAbilitySystemComponent = Cast<UUB_AbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(this));
		
	}
	return BusterAbilitySystemComponent;
}


 	


void AUB_BaseCharacter::BroadcastInitialValues()
{
	if (const UUB_AttributeSet* AS = Cast<UUB_AttributeSet>(AttributeSet))
	{
		
		OnHealthChanged.Broadcast(AS->GetHealth());
		OnMaxHealthChanged.Broadcast(AS->GetMaxHealth());

		OnAmmoChanged.Broadcast(AS->GetAmmo());
		OnMaxAmmoChanged.Broadcast(AS->GetMaxAmmo());
	}
}

void AUB_BaseCharacter::BindCallbacksToDependencies()
{
	if (const UUB_AttributeSet* AS = Cast<UUB_AttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetAmmoAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnAmmoChanged.Broadcast(Data.NewValue);
		}
	);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetMaxAmmoAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxAmmoChanged.Broadcast(Data.NewValue);
			}
		);	
 
	}

}

void AUB_BaseCharacter::InitHealthBarWidget()
{
	if (HealthBar)
	{
		
		HealthBar->InitWidget(); 

		if (UBuster_UserWidget* BusterUserWidget = Cast<UBuster_UserWidget>(HealthBar->GetUserWidgetObject()))
		{
			BusterUserWidget->SetWidgetController(this);
		}
	}
}

void AUB_BaseCharacter::InitAmmoWidget()
{
	if (AmmoWidget)
	{
		
		AmmoWidget->InitWidget(); 

		if (UBuster_UserWidget* BusterUserWidget = Cast<UBuster_UserWidget>(AmmoWidget->GetUserWidgetObject()))
		{
			BusterUserWidget->SetWidgetController(this);
		}
	}
}

//Deorecated
void AUB_BaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(AUB_BaseCharacter, InventoryComponent);
}


