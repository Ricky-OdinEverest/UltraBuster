// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AttributeInfo.h"

#include "UB_GameplayTags.h"
#include "AbilitySystem/Attributes/UB_AttributeSet.h"

FUB_AttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	for (const FUB_AttributeInfo& Info : AttributeInformation)
	{
		if (Info.AttributeTag.MatchesTagExact(AttributeTag))
		{
			return Info;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find Info for AttributeTag [%s] on AttributeInfo [%s]."), *AttributeTag.ToString(),*GetNameSafe(this));
	}

	return FUB_AttributeInfo();
}
#if WITH_EDITOR

void UAttributeInfo::PopulateDataAsset()
{
	const FUB_GameplayTags SCR_GameplayTags = FUB_GameplayTags::Get();
	AttributeInformation.Empty();
	AttributeInformation.SetNumZeroed(SCR_GameplayTags.UB_TagsContainer.Num());

	for (int i = 0; i < SCR_GameplayTags.UB_TagsContainer.Num(); i++)
	{
		const FGameplayTag IndexTag = SCR_GameplayTags.UB_TagsContainer.GetByIndex(i);
		AttributeInformation[i].AttributeTag = IndexTag; // SetAttributeTag

		const FGameplayTagNode* TagNode = UGameplayTagsManager::Get().FindTagNode(IndexTag).Get(); // Get the last node of the gameplay tag
		FString NodeString = FName::NameToDisplayString(TagNode->GetSimpleTagName().ToString(), false); // Get Node Name (Attribute.Primary.Strength returns Strength)

		AttributeInformation[i].AttributeName = FText::FromString(NodeString); //Set Attributes

		//Set Attributes To Get
		for (TFieldIterator<FProperty> It(UUB_AttributeSet::StaticClass()); It; ++It)
		{
			if(FGameplayAttribute::IsGameplayAttributeDataProperty(*It) && FGameplayAttribute(*It).GetName() == TagNode->GetSimpleTagName().ToString())
			{
				AttributeInformation[i].AttributeToGet = FGameplayAttribute(*It);
				break;
			}
		}

		const FName TagName = IndexTag.GetTagName();
		// Outs
		FString TagComment = FString();
		FName TagSource = FName();
		bool bIsTagExplicit = false;
		bool bIsRestrictedTag = false;
		bool bAllowNonRestrictedChildren = false;

		UGameplayTagsManager::Get().GetTagEditorData(
			TagName,
			TagComment,
			TagSource,
			bIsTagExplicit,
			bIsRestrictedTag,
			bAllowNonRestrictedChildren);
		AttributeInformation[i].AttributeDescription = FText::FromString(TagComment); // Set Attribute Description
	}
	
}
#endif

