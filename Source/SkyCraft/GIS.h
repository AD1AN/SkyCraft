// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "GIS.generated.h"

class APCS;
class APSS;

UCLASS()
class SKYCRAFT_API UGIS : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()
	
public:	
	UGIS(FObjectInitializer const & ObjectInitializerRef) : Super(ObjectInitializerRef){}

	UPROPERTY(BlueprintReadOnly) APCS* PCS = nullptr;
	UPROPERTY(BlueprintReadOnly) APSS* PSS = nullptr;
};