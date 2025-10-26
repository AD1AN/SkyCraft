// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Structs/CharacterBio.h"
#include "LocalSettings.generated.h"

class UDA_Foliage;

UCLASS()
class SKYCRAFT_API ULocalSettings : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite) bool bDemoNoticeAgreed = false;
	
	UPROPERTY(BlueprintReadWrite) bool bMattockEditingMode = false;
	UPROPERTY(BlueprintReadWrite) TObjectPtr<UDA_Foliage> MattockFoliage;

	UPROPERTY(BlueprintReadWrite) float FOV = 90;
	UPROPERTY(BlueprintReadWrite) float MasterVolume = 1;
	UPROPERTY(BlueprintReadWrite) float GameVolume = 1;
	UPROPERTY(BlueprintReadWrite) float InterfaceVolume = 1;
	UPROPERTY(BlueprintReadWrite) float MouseSensitivity = 1;
	UPROPERTY(BlueprintReadWrite) float CameraSensitivity = 1;
	UPROPERTY(BlueprintReadWrite) bool bInvertVerticalMouse = false;

	UPROPERTY(BlueprintReadWrite) FCharacterBio CharacterBio;

	void CreateRandomCharacterBio()
	{
		CharacterBio.Gender = FMath::RandBool();
		CharacterBio.HairStyle = 0;
		FLinearColor HairHSV(FMath::RandRange(0.0f, 360.0f), FMath::RandRange(0.0f, 0.75f), FMath::RandRange(0.0f, 1.0f), 1);
		CharacterBio.HairColor = HairHSV.HSVToLinearRGB();
		FLinearColor EyeHSV(FMath::RandRange(0.0f, 360.0f), FMath::RandRange(0.0f, 1.0f), FMath::RandRange(0.25f, 1.0f), 1);
		CharacterBio.EyeColor = EyeHSV.HSVToLinearRGB();
		CharacterBio.SkinTone = FMath::RandRange(0.9f, 1.0f);
	}
};
