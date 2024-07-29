// ADIAN Copyrighted

#include "GSS.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Structs/FX.h"

AGSS::AGSS(){}

void AGSS::Multicast_SpawnFXAttached_Implementation(FFX FX, FVector LocalLocation, USceneComponent* AttachTo, USoundAttenuation* AttenuationSettings)
{
	if (!IsValid(AttachTo)) return;
	if (FX.Sound) UGameplayStatics::SpawnSoundAttached(FX.Sound, AttachTo, NAME_None, LocalLocation, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, 1,1,0,AttenuationSettings);
	if (FX.Niagara)
	{
		UNiagaraComponent* SpawnedNiagara;
		SpawnedNiagara = UNiagaraFunctionLibrary::SpawnSystemAttached(FX.Niagara, AttachTo, NAME_None, LocalLocation, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
		if (FX.bHaveNiagaraVars)
		{
			for (FNiagaraVar NiagaraVar : FX.NiagaraVars)
			{
				if (NiagaraVar.Type == ENiagaraVarType::Float) SpawnedNiagara->SetVariableFloat(NiagaraVar.Name, NiagaraVar.Float);
				else if (NiagaraVar.Type == ENiagaraVarType::Vector) SpawnedNiagara->SetVariableVec3(NiagaraVar.Name, NiagaraVar.Vector);
				else if (NiagaraVar.Type == ENiagaraVarType::Color) SpawnedNiagara->SetVariableLinearColor(NiagaraVar.Name, NiagaraVar.Color);
				else if (NiagaraVar.Type == ENiagaraVarType::StaticMesh)
				{
					if (IsValid(NiagaraVar.StaticMesh))
					{
						UNiagaraFunctionLibrary::OverrideSystemUserVariableStaticMesh(SpawnedNiagara, NiagaraVar.Name.ToString(), NiagaraVar.StaticMesh);
					}
				}
			}
		}
	}
}