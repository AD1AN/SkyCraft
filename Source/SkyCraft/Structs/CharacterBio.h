#pragma once

#include "CharacterBio.generated.h"

USTRUCT(BlueprintType)
struct FCharacterBio
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Gender = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 HairStyle = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor HairColor = FLinearColor(0.0f,0.0f,0.0f,1.0f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor EyeColor = FLinearColor(0.0f,0.0f,0.0f,1.0f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SkinTone = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDateTime DateTimeCreated;
};