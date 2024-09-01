// ADIAN Copyrighted

#include "Constellation.h"
#include "NiagaraComponent.h"

AConstellation::AConstellation()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bNetUseOwnerRelevancy = true;
	SetReplicates(true);
}

void AConstellation::SetPreviewColor(bool Allowed)
{
	FLinearColor EssenceColor;
	if (Allowed) EssenceColor = FLinearColor::Green;
	else EssenceColor = FLinearColor::Red;

	UNiagaraComponent* Niagara = FindComponentByClass<UNiagaraComponent>();
	if (IsValid(Niagara))
	{
		Niagara->SetVariableLinearColor("EssenceColor", EssenceColor);
	}
	
	// TArray<UNiagaraComponent*> Niagaras;
	// GetComponents<UNiagaraComponent>(Niagaras);
	// for (UNiagaraComponent* niagara : Niagaras)
	// {
	// 	niagara->SetVariableLinearColor("EssenceColor", EssenceColor);
	// }
}
