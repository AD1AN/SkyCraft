// Copyright Epic Games, Inc. All Rights Reserved.

#include "K2Node_AdianPlayMontage.h"
#include "Containers/UnrealString.h"
#include "EdGraph/EdGraphPin.h"
#include "HAL/Platform.h"
#include "Internationalization/Internationalization.h"
#include "Misc/AssertionMacros.h"
#include "AdianPlayMontageCallbackProxy.h"
#include "UObject/NameTypes.h"
#include "UObject/ObjectPtr.h"

#define LOCTEXT_NAMESPACE "K2Node"

UK2Node_AdianPlayMontage::UK2Node_AdianPlayMontage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UAdianPlayMontageCallbackProxy, CreateProxyObjectForPlayMontage);
	ProxyFactoryClass = UAdianPlayMontageCallbackProxy::StaticClass();
	ProxyClass = UAdianPlayMontageCallbackProxy::StaticClass();
}

FText UK2Node_AdianPlayMontage::GetTooltipText() const
{
	return LOCTEXT("K2Node_AdianPlayMontage_Tooltip", "Plays a Montage on a SkeletalMeshComponent");
}

FText UK2Node_AdianPlayMontage::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("AdianPlayMontage", "Adian Play Montage");
}

FText UK2Node_AdianPlayMontage::GetMenuCategory() const
{
	return LOCTEXT("AdianPlayMontageCategory", "Animation|Montage");
}

void UK2Node_AdianPlayMontage::GetPinHoverText(const UEdGraphPin& Pin, FString& HoverTextOut) const
{
	Super::GetPinHoverText(Pin, HoverTextOut);

	static const FName NAME_OnNotifyBegin = FName(TEXT("OnNotifyBegin"));
	static const FName NAME_OnNotifyEnd = FName(TEXT("OnNotifyEnd"));

	if (Pin.PinName == NAME_OnNotifyBegin)
	{
		FText ToolTipText = LOCTEXT("K2Node_AdianPlayMontage_OnNotifyBegin_Tooltip", "Event called when using a PlayMontageNotify or PlayMontageNotifyWindow Notify in a Montage.");
		HoverTextOut = FString::Printf(TEXT("%s\n%s"), *ToolTipText.ToString(), *HoverTextOut);
	}
	else if (Pin.PinName == NAME_OnNotifyEnd)
	{
		FText ToolTipText = LOCTEXT("K2Node_AdianPlayMontage_OnNotifyEnd_Tooltip", "Event called when using a PlayMontageNotifyWindow Notify in a Montage.");
		HoverTextOut = FString::Printf(TEXT("%s\n%s"), *ToolTipText.ToString(), *HoverTextOut);
	}
}

#undef LOCTEXT_NAMESPACE
