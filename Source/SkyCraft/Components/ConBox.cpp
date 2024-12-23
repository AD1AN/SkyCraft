// ADIAN Copyrighted

#include "ConBox.h"

UConBox::UConBox()
{
	ShapeColor = FColor::Blue;
	LineThickness = 1.0f;
	UPrimitiveComponent::SetCollisionProfileName(TEXT("ConBox"));
}