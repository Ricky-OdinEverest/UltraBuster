// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeWidgetController.h"

#include "AbilitySystem/UB_AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/UB_AttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"


void UAttributeWidgetController::BindCallbacksToDependencies()
{
	UUB_AttributeSet* AS = CastChecked<UUB_AttributeSet>(AttributeSet);
	check(AttributeInfo);
	for (FUB_AttributeInfo& Info : AttributeInfo->AttributeInformation)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Info.AttributeToGet).AddLambda(
		[this, Info](const FOnAttributeChangeData& Data)
		{
			BroadcastAttributeInfo(Info.AttributeTag);
		}
	);
	}
}

void UAttributeWidgetController::BroadcastInitialValues()
{
	UUB_AttributeSet* AS = CastChecked<UUB_AttributeSet>(AttributeSet);

	check(AttributeInfo);
	
	for (FUB_AttributeInfo& Info : AttributeInfo->AttributeInformation)
	{
		BroadcastAttributeInfo(Info.AttributeTag);
	}
}

//Upgrade a single attribute...Should Probably transition to a vecotr or TARRAY
void UAttributeWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	UUB_AbilitySystemComponent* BusterASC = CastChecked<UUB_AbilitySystemComponent>(AbilitySystemComponent);
	BusterASC->UpgradeAttribute(AttributeTag);
}

void UAttributeWidgetController::BroadcastAttributeInfoOrg(const FGameplayTag& AttributeTag,
                                                           const FGameplayAttribute& Attribute) const
{
	FUB_AttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}

void UAttributeWidgetController::BroadcastAttributeInfo(const FGameplayTag& Tag) const
{
	//getting info from Data Asset Attribute Info based on Gameplay Tag
	FUB_AttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Tag);
	//Set Hidden Value in AttributeInfo Data Asset
	Info.AttributeValue = Info.AttributeToGet.GetNumericValue(AttributeSet);
	//Broadcast for those who subscribe. IE the attribute menu widget in blueprint
	AttributeInfoDelegate.Broadcast(Info);
}
