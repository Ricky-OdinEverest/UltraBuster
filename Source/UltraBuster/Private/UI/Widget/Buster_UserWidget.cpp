// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/Buster_UserWidget.h"

void UBuster_UserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}