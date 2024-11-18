// ADIAN Copyrighted

#include "AdianWidget.h"

UAdianWidget::UAdianWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UAdianWidget::NativeOnInitialized()
{
	PreInitialized();
	Super::NativeOnInitialized();
	PostInitialized();
}

void UAdianWidget::NativeConstruct()
{
	if (!bConstructOnce)
	{
		bConstructOnce = true;
		InitialConstruct();
		ConstructOnce();
	}
	Super::NativeConstruct();
}
