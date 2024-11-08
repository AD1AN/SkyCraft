// Staz Lincord Copyrighted


#include "PCS.h"
#include "GIS.h"

void APCS::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController()) return;
	if (UGIS* GIS = Cast<UGIS>(GetGameInstance()))
	{
		GIS->PCS = this;
	}
}


void APCS::PawnLeavingGame()
{
	
}

bool APCS::IsPawnInputEnabled(APawn* CheckPawn)
{
	return CheckPawn->InputEnabled();
}