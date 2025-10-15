#pragma once

#include "CharacterBio.generated.h"

USTRUCT(BlueprintType)
struct FCharacterBio
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Gender = true; // True = Male, False = Female

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

	FORCEINLINE bool operator==(const FCharacterBio& Other) const
	{
		return CharacterName == Other.CharacterName
			&& Gender == Other.Gender
			&& HairStyle == Other.HairStyle
			&& HairColor == Other.HairColor
			&& EyeColor == Other.EyeColor
			&& FMath::IsNearlyEqual(SkinTone, Other.SkinTone)
			&& DateTimeCreated == Other.DateTimeCreated;
	}

	FORCEINLINE bool operator!=(const FCharacterBio& Other) const
	{
		return !(*this == Other);
	}
};