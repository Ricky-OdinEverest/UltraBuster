// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Buster_UserWidget.generated.h"

/**
 * Base class for user widgets 
 * lets you set the specific widget controller for a widget 
 */
UCLASS()
class ULTRABUSTER_API UBuster_UserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;

protected:
	//Called after widget controller is set to ensure we have access to the
	//widget controller and corresponding data 
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
};
