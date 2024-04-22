// ADIAN Copyrighted

#include "AdianFL.h"

FLinearColor UAdianFL::EssenceToRGB(const FEssence& Essence)
{
	const uint32 EssencePositiveMax = FMath::Max3(Essence.Red, Essence.Green, Essence.Blue);
	if (EssencePositiveMax == 0)
	{
		return FLinearColor(0.5f,0.5f,0.5f,1.f);
	}
	FLinearColor RGB;
	RGB.R = static_cast<float>(Essence.Red) / EssencePositiveMax;
	RGB.G = static_cast<float>(Essence.Green) / EssencePositiveMax;
	RGB.B = static_cast<float>(Essence.Blue) / EssencePositiveMax;
	RGB.A = 1.f;
	return RGB;
}