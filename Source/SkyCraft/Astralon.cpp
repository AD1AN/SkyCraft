// ADIAN Copyrighted

#include "Astralon.h"
#include "NiagaraComponent.h"
#include "Components/InteractComponent.h"

AAstralon::AAstralon()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bNetUseOwnerRelevancy = true;
	bReplicates = true;
	
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	SetRootComponent(NiagaraComponent);
	
	InteractComponent = CreateDefaultSubobject<UInteractComponent>(TEXT("InteractComponent"));
}

void AAstralon::SetPreviewColor(bool Allowed)
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
