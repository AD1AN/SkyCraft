// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "GIS.generated.h"

class UInputMappingContext;
class ULocalSettings;
class APCS;
class APSS;

UCLASS()
class SKYCRAFT_API UGIS : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly) TObjectPtr<ULocalSettings> LocalSettings;
	UFUNCTION(BlueprintCallable) void SaveLocalSettings();

	UPROPERTY(EditDefaultsOnly) TObjectPtr<UInputMappingContext> DefaultMappingContext;
	
	UGIS(FObjectInitializer const & ObjectInitializerRef) : Super(ObjectInitializerRef){}

	UPROPERTY(BlueprintReadOnly) APCS* PCS = nullptr;
	UPROPERTY(BlueprintReadOnly) APSS* PSS = nullptr;

	UPROPERTY(BlueprintReadWrite) FString LoadWorldName;

	// >>>>>>>>>>>>>>>>>>>>>>> World Creation Settings
	UPROPERTY(BlueprintReadWrite) bool bHostStartAsArchon = true;
	UPROPERTY(BlueprintReadWrite) bool bHostPlayerIslandIsCrystal = true;
	// <<<<<<<<<<<<<<<<<<<<<<< World Creation Settings

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly) bool DebugChunks = false;
	UPROPERTY(EditDefaultsOnly) bool bTestChunkIslandRenderRange = false;
	UPROPERTY(EditDefaultsOnly) int32 TestChunkIslandRenderRange = 10;
#endif

	virtual void Init() override;
};