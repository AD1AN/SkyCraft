// ADIAN Copyrighted

#include "SnapBox.h"

USnapBox::USnapBox()
{
	ShapeColor = FColor::Green;
	UPrimitiveComponent::SetCollisionProfileName(TEXT("SnapBox"));
}

void USnapBox::OnRegister() // IDK how to move box extent, I'm moving the component.
{
	Super::OnRegister();
	FTransform CurrentTransform = GetComponentTransform();
	FVector NewLocation = CurrentTransform.TransformPosition(BoxOffset);
	CurrentTransform.SetLocation(NewLocation);
	SetWorldTransform(CurrentTransform);
}