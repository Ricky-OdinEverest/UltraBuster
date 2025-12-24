// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/Buster_WidgetController.h"

void UBuster_WidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UBuster_WidgetController::BroadcastInitialValues()
{
}

void UBuster_WidgetController::BindCallbacksToDependencies()
{
}

