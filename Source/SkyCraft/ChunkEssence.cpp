// ADIAN Copyrighted

#include "ChunkEssence.h"

#include "AdianFL.h"
#include "GSS.h"

void AChunkEssence::BeginActor_Implementation()
{
	Super::BeginActor_Implementation();
	GSS = GetWorld()->GetGameState<AGSS>();
}

void AChunkEssence::RandomEssence(int32& Essence, FLinearColor& EssenceColor)
{
	// White
	if (UAdianFL::RandomBoolWithWeight(0.2f))
	{
		Essence = FMath::RandRange(600, 700);
		EssenceColor = FLinearColor::White;
	}
	// Blue
	else if (UAdianFL::RandomBoolWithWeight(0.75f))
	{
		Essence = FMath::RandRange(100, 300);
		EssenceColor = FLinearColor(0.25f, 0.25f, 1.0f, 1.0f);
	}
	// Green
	else if (UAdianFL::RandomBoolWithWeight(0.6f))
	{
		Essence = FMath::RandRange(400, 600);
		EssenceColor = FLinearColor(0.25f, 1.0f, 0.25f, 1.0f);
	}
	// Red
	else
	{
		Essence = FMath::RandRange(1000, 1200);
		EssenceColor = FLinearColor(1.0f, 0.1f, 0.1f, 1.0f);
	}
}
