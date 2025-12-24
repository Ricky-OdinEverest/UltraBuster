// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/BusterHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/Widget/Buster_UserWidget.h"
#include "UI/WidgetController/AttributeWidgetController.h"
#include "UI/WidgetController/OverlayWidgetController.h"

UAttributeWidgetController* ABusterHUD::GetAttributeWidgetController(const FWidgetControllerParams& WCParams)
{
	if (AttributeWidgetController == nullptr)
	{
		AttributeWidgetController = NewObject<UAttributeWidgetController>(this, AttributeWidgetControllerClass);
		AttributeWidgetController->SetWidgetControllerParams(WCParams);
		AttributeWidgetController->BindCallbacksToDependencies();
	}
	return AttributeWidgetController;
}
UOverlayWidgetController* ABusterHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		//OverlayWidgetController->BindCallbacksToDependencies();
		return OverlayWidgetController;
	}
	return OverlayWidgetController;
}
// Currently Call By Player
void ABusterHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class uninitialized, please fill out BP_BusterHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class uninitialized, please fill out BP_BusterHUD"));

	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);

	OverlayWidget = Cast<UBuster_UserWidget>(Widget);

	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);

	//widget controller set bp called after
	OverlayWidget->SetWidgetController(WidgetController);
	
	//WidgetController->BroadcastInitialValues();
	Widget->AddToViewport();
}


