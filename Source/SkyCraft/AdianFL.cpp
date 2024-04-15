// ADIAN Copyrighted

#include "AdianFL.h"

FLinearColor UAdianFL::EssenceToRGB(const FEssence& Essence)
{
	FLinearColor RGB;
	int32 EssencePositiveMax = FMath::Max3(Essence.Red, Essence.Green, Essence.Blue);

	float positive_r = Essence.Red / EssenceTotal(Essence);
	float positive_g = Essence.Green / EssenceTotal(Essence);
	float positive_b = Essence.Blue / EssenceTotal(Essence);

	RGB.R = FMath::Clamp(positive_r, 0.f, 1.f);
	RGB.G = FMath::Clamp(positive_g, 0.f, 1.f);
	RGB.B = FMath::Clamp(positive_b, 0.f, 1.f);
	RGB.A = 1.f;

	return RGB;
}
