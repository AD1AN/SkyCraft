// ADIAN Copyrighted

#include "PlacedObject.h"
#include "HealthSystem.h"

APlacedObject::APlacedObject()
{
	HealthSystem = CreateDefaultSubobject<UHealthSystem>(TEXT("HealthSystem"));
}