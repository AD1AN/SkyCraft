#pragma once

#include "IntMinMax.h"
#include "Essence.generated.h"

USTRUCT(BlueprintType)
struct FEssence
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 R = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 G = 0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) int32 B = 0;

	int32 Total() const { return R+G+B; }
	
	FEssence& operator-=(int32 TotalToSubtract)
	{
		int32 TotalSum = Total();
		if (TotalSum == 0 || TotalToSubtract <= 0) return *this;

		// Clamp to available amount
		TotalToSubtract = FMath::Min(TotalToSubtract, TotalSum);

		const float Proportion = static_cast<float>(TotalToSubtract) / static_cast<float>(TotalSum);

		// Calculate proportional subtraction
		int32 SubR = FMath::Clamp(FMath::FloorToInt(R * Proportion), 0, R);
		int32 SubG = FMath::Clamp(FMath::FloorToInt(G * Proportion), 0, G);
		int32 SubB = FMath::Clamp(FMath::FloorToInt(B * Proportion), 0, B);

		R -= SubR;
		G -= SubG;
		B -= SubB;

		// Adjust for rounding errors
		int32 Difference = TotalToSubtract - (SubR + SubG + SubB);

		while (Difference > 0)
		{
			// Pick the largest channel, to subtract with leftovers
			if (R >= G && R >= B && R > 0) --R;
			else if (G >= B && G > 0) --G;
			else if (B > 0) --B;
			else break; // Nothing left to adjust
			--Difference;
		}

		return *this;
	}
	
	FEssence operator-(int32 TotalToSubtract) const
	{
		FEssence Result = *this;
		Result -= TotalToSubtract;
		return Result;
	}

	bool operator==(const FEssence& Other) const
	{
		return R == Other.R && G == Other.G && B == Other.B;
	}

	bool operator!=(const FEssence& Other) const
	{
		return !(*this == Other);
	}
};

UENUM()
enum class EDropEssenceAmount : uint8
{
	MinMax,
	Static
};

UENUM()
enum class EDropEssenceLocationType : uint8
{
	ActorOriginPlusZ,
	OverrideLocation
};

USTRUCT(BlueprintType)
struct FEssenceMinMax
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere) FIntMinMax R;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FIntMinMax G;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) FIntMinMax B;
};