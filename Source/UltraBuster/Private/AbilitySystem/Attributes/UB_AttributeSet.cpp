// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/UB_AttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

UUB_AttributeSet::UUB_AttributeSet()
{
	InitHealth(100.f);
	InitMaxHealth(100.f);
}

void UUB_AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UUB_AttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UUB_AttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UUB_AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
}

void UUB_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	FEffectProperties Props;
	SetEffectProperties(Data, Props);

	//if(Props.TargetCharacter->Implements<UCombatInterface>() && ICombatInterface::Execute_IsDead(Props.TargetCharacter)) return;

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}

}

void UUB_AttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUB_AttributeSet, Health, OldHealth);
}

void UUB_AttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UUB_AttributeSet, MaxHealth, OldMaxHealth);
}

void UUB_AttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	// Source = causer of the effect, Target = target of the effect (owner of this AS)

	// Store the GameplayEffectContext from the incoming EffectSpec (contains all contextual info about the effect).
	Props.EffectContextHandle = Data.EffectSpec.GetContext();

	// Get the source (instigator's) AbilitySystemComponent from the context.
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	// Validate that the SourceASC exists and that the associated ActorInfo and AvatarActor are valid.
	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		// Get the source's avatar actor (e.g., character or pawn who caused the effect).
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();

		// Get the PlayerController associated with the source (if available).
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();

		// If no controller was directly found, try to get it from the avatar (e.g., from a pawn).
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			// Cast the avatar to APawn and try to get its controller.
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}

		// If we successfully found a controller, get the pawn it is controlling and cast it to a character.
		Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
	}

	// Now gather info about the target of the effect (usually the actor whose AttributeSet is being modified).
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		// Get the target's avatar actor (e.g., the player or NPC receiving the effect).
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();

		// Get the controller associated with the target.
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();

		// Attempt to cast the avatar actor to an ACharacter for further use (e.g., animations, hit reactions).
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);

		// Retrieve the AbilitySystemComponent from the target's avatar actor using the blueprint library helper.
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}

void UUB_AttributeSet::ShowFloatingText(const FEffectProperties& Props, float Damage, bool bBlockedHit,
	bool bCriticalHit) const
{
}
