// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TESTInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTESTInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SKYCRAFT_API ITESTInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	void TestingInterface();
};
