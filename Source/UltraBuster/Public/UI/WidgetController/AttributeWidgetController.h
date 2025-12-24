// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/Buster_WidgetController.h"
#include "AttributeWidgetController.generated.h"

struct FGameplayAttribute;
struct FGameplayTag;
class UAttributeInfo;
struct FUB_AttributeInfo;

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSecondaryAttributeChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FUB_AttributeInfo&, Info);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class ULTRABUSTER_API UAttributeWidgetController : public UBuster_WidgetController
{
	GENERATED_BODY()
public:
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;

	UFUNCTION(BlueprintCallable)
	void UpgradeAttribute(const FGameplayTag& AttributeTag);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Widget Data")
	TObjectPtr<UAttributeInfo> AttributeInfo;

	
private:

	//OriginalFunc
	void BroadcastAttributeInfoOrg(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const;

	//Helper func to broadcast attributes alternative solution
	void BroadcastAttributeInfo(const FGameplayTag& Tag) const;

};