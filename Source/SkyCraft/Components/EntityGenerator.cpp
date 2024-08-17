// ADIAN Copyrighted

#include "EntityGenerator.h"
#include "Math/RandomStream.h"
#include "HealthSystem.h"
#include "SkyCraft/AdianFL.h"
#include "SkyCraft/Island.h"
#include "SkyCraft/Structs/SS_IslandStatic.h"
#include "SkyCraft/Resource.h"
#include "SkyCraft/NPC.h"
#include "SkyCraft/DataAssets/DA_Resource.h"

UEntityGenerator::UEntityGenerator()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}
void UEntityGenerator::SetupGenerator(FSetupGeneratorIn SetupGeneratorIn)
{
	AreaSize = SetupGeneratorIn.AreaSize;
	GeneratorSeed = SetupGeneratorIn.GeneratorSeed;
	ScaleRatio = SetupGeneratorIn.ScaleRatio;
	GroundAltitude = SetupGeneratorIn.GroundAltitude;
	LinetraceLength = SetupGeneratorIn.LinetraceLength;
}
void UEntityGenerator::GenerateResources(FGenerateResourcesIn GenerateResourcesIn)
{
	if (!IsValid(GenerateResourcesIn.DA_Resource)) return;
	const uint32 _SpawnPoints = GenerateResourcesIn.MaxSpawnPoints / ((3-ScaleRatio) - (ScaleRatio));
	const float _GridSize = sqrt(_SpawnPoints);
	const float _CellSize = AreaSize / _GridSize;
	const float _MaxOffset = _CellSize * 0.25f;

	FVector ActorLoc = GetOwner()->GetActorLocation();

	FRandomStream _StreamX = GeneratorSeed + Generations;
	FRandomStream _StreamY = GeneratorSeed + Generations + 1;

	const float _IslandScale = FMath::Clamp(ScaleRatio, 0.25f, 0.5f);

	FEntities* FoundLOD = SpawnedLODs.Find(GenerateResourcesIn.LOD);
	if (!FoundLOD) FoundLOD = &SpawnedLODs.Add(GenerateResourcesIn.LOD, FEntities{});
	
	for (uint16 _Row = 0; _Row < _GridSize-1; ++_Row)
	{
		for (uint16 _Column = 0; _Column < _GridSize-1; ++_Column)
		{
			FVector LocalLoc = FVector(
				_StreamX.FRandRange(_MaxOffset * -1.f, _MaxOffset) + (_CellSize * _Column + _CellSize * 0.5f) * _IslandScale,
				_StreamY.FRandRange(_MaxOffset * -1.f, _MaxOffset) + (_CellSize * _Row + _CellSize * 0.5f) * _IslandScale,
				GroundAltitude);
			FVector WorldLoc = LocalLoc + ActorLoc;
			float ScaleXY = _IslandScale * (AreaSize / 2.f);
			FVector StartLoc = WorldLoc - FVector(ScaleXY, ScaleXY, 0);
			FVector EndLoc = StartLoc;
			EndLoc.Z += LinetraceLength;

			if (FHitResult HitResult; GetWorld()->LineTraceSingleByChannel(HitResult, StartLoc, EndLoc, ECC_Visibility))
			{
				if (UAdianFL::ActorHasSkyTags(HitResult.GetActor(), CollisionSkyTags))
				{
					if (FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(HitResult.ImpactNormal, FVector::UpVector))) < GenerateResourcesIn.DA_Resource->MaxFloorSlope)
					{
						FTransform ResTransform;
						ResTransform.SetLocation(HitResult.ImpactPoint);
						ResTransform.SetRotation(FQuat(FRotator(0,_StreamX.FRandRange(-359.0f, 359.0f),0)));
						TSubclassOf<AResource> ResourceClass = (GenerateResourcesIn.DA_Resource->OverrideResourceClass) ? GenerateResourcesIn.DA_Resource->OverrideResourceClass : TSubclassOf<AResource>(AResource::StaticClass());
						AResource* SpawnedRes = GetWorld()->SpawnActorDeferred<AResource>(ResourceClass, ResTransform);
						if (!SpawnedRes) continue;
						SpawnedRes->DA_Resource = GenerateResourcesIn.DA_Resource;
						uint8 ResSize = _StreamY.RandRange(GenerateResourcesIn.ResourceSize.Min, GenerateResourcesIn.ResourceSize.Max);
						SpawnedRes->ResourceSize = ResSize;
						uint8 VarietyNum = GenerateResourcesIn.DA_Resource->Size[ResSize].SM_Variety.Num();
						SpawnedRes->SM_Variety = (VarietyNum >= 1) ? _StreamX.RandRange(0, VarietyNum-1) : 0;
						SpawnedRes->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepWorldTransform);
						SpawnedRes->FinishSpawning(ResTransform);
						FoundLOD->Resources.Add(SpawnedRes);
					}
				}
			}
			_StreamX = _StreamX.GetUnsignedInt();
			_StreamY = _StreamY.GetUnsignedInt();
			
			// UE_LOG(LogTemp, Warning, TEXT("%f"), FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(HitResult.ImpactNormal, FVector::UpVector))));
			// DrawDebugLine(GetWorld(), StartLoc, EndLoc, FColor::Red, false, 555);
		}
	}
	Generations++;
}
void UEntityGenerator::GenerateNPCs(FGenerateNPCsIn GenerateNPCsIn)
{
	if (!GenerateNPCsIn.NPC_Class) return;
	const uint32 _SpawnPoints = GenerateNPCsIn.MaxSpawnPoints / ((3-ScaleRatio) - (ScaleRatio));
	const float _GridSize = sqrt(_SpawnPoints);
	const float _CellSize = AreaSize / _GridSize;
	const float _MaxOffset = _CellSize * 0.25f;

	if (!IsValid(GetOwner())) return;
	AIsland* Island = Cast<AIsland>(GetOwner());
	FVector ActorLoc = GetOwner()->GetActorLocation();

	FRandomStream _StreamX = GeneratorSeed + Generations;
	FRandomStream _StreamY = GeneratorSeed + Generations + 1;

	const float _IslandScale = FMath::Clamp(ScaleRatio, 0.25f, 0.5f);
	
	FEntities* FoundLOD = SpawnedLODs.Find(GenerateNPCsIn.LOD);
	if (!FoundLOD) FoundLOD = &SpawnedLODs.Add(GenerateNPCsIn.LOD, FEntities{});

	for (uint16 _Row = 0; _Row < _GridSize-1; ++_Row)
	{
		for (uint16 _Column = 0; _Column < _GridSize-1; ++_Column)
		{
			FVector LocalLoc = FVector(
				_StreamX.FRandRange(_MaxOffset * -1.f, _MaxOffset) + (_CellSize * _Column + _CellSize * 0.5f) * _IslandScale,
				_StreamY.FRandRange(_MaxOffset * -1.f, _MaxOffset) + (_CellSize * _Row + _CellSize * 0.5f) * _IslandScale,
				GroundAltitude);
			FVector WorldLoc = LocalLoc + ActorLoc;
			float ScaleXY = _IslandScale * (AreaSize / 2.f);
			FVector StartLoc = WorldLoc - FVector(ScaleXY, ScaleXY, 0);
			FVector EndLoc = StartLoc;
			EndLoc.Z += LinetraceLength;

			if (FHitResult HitResult; GetWorld()->LineTraceSingleByChannel(HitResult, StartLoc, EndLoc, ECC_Visibility))
			{
				if (UAdianFL::ActorHasSkyTags(HitResult.GetActor(), CollisionSkyTags))
				{
					if (FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(HitResult.ImpactNormal, FVector::UpVector))) < GenerateNPCsIn.MaxFloorSlope)
					{
						FTransform ResTransform;
						ResTransform.SetLocation(HitResult.ImpactPoint + FVector(0,0,100));
						ResTransform.SetRotation(FQuat(FRotator(0,_StreamX.FRandRange(-359.0f, 359.0f),0)));
						ANPC* SpawnedNPC = GetWorld()->SpawnActorDeferred<ANPC>(GenerateNPCsIn.NPC_Class, ResTransform);
						SpawnedNPC->Island = Island;
						SpawnedNPC->FinishSpawning(ResTransform);
						FoundLOD->NPCs.Add(SpawnedNPC);
					}
				}
			}
			_StreamX = _StreamX.GetUnsignedInt();
			_StreamY = _StreamY.GetUnsignedInt();
		}
	}
	Generations++;
}
TArray<FSS_IslandLOD> UEntityGenerator::SaveLODs()
{
	TArray<FSS_IslandLOD> SavedLODs;
	for (const TPair<int32, FEntities>& LOD : SpawnedLODs)
	{
		FSS_IslandLOD SavingLOD;
		SavingLOD.LOD = LOD.Key;
		
		for (AResource* Res : LOD.Value.Resources)
		{
			if (!IsValid(Res)) continue;
			FSS_Resource SS_Res;
			SS_Res.RelativeLocation = Res->StaticMeshComponent->GetRelativeLocation();
			SS_Res.RelativeRotation = Res->StaticMeshComponent->GetRelativeRotation();
			SS_Res.DA_Resource = Res->DA_Resource;
			SS_Res.ResourceSize = Res->ResourceSize;
			SS_Res.SM_Variety = Res->SM_Variety;
			SS_Res.Health = Res->HealthSystem->Health;
			SS_Res.Growing = Res->Growing;
			SS_Res.GrowMarkTime = Res->GrowMarkTime;
			SS_Res.GrowSavedTime = Res->GrowSavedTime;
			SavingLOD.Resources.Add(SS_Res);
		}
		
		for (ANPC* NPC : LOD.Value.NPCs)
		{
			if (!IsValid(NPC)) continue;
			FSS_NPC SS_NPC;
			SS_NPC = NPC->SaveNPC();
			SavingLOD.NPCs.Add(SS_NPC);
		}
		
		SavedLODs.Add(SavingLOD);
	}
	return SavedLODs;
}
void UEntityGenerator::LoadResources(TArray<FSS_Resource> Resources, int32 LOD)
{
	if (Resources.IsEmpty()) return;
	FEntities* SpawnedLOD = SpawnedLODs.Find(LOD);
	if (!SpawnedLOD) SpawnedLOD = &SpawnedLODs.Add(LOD, FEntities{});
	for (const FSS_Resource Res : Resources)
	{
		if (!Res.DA_Resource) continue;
		FTransform ResTransform;
		ResTransform.SetLocation(Res.RelativeLocation);
		ResTransform.SetRotation(FQuat(Res.RelativeRotation));
		TSubclassOf<AResource> ResourceClass = (Res.DA_Resource->OverrideResourceClass) ? Res.DA_Resource->OverrideResourceClass : TSubclassOf<AResource>(AResource::StaticClass());
		AResource* SpawnedRes = GetWorld()->SpawnActorDeferred<AResource>(ResourceClass, ResTransform);
		SpawnedRes->bLoaded = true;
		SpawnedRes->LoadHealth = Res.Health;
		SpawnedRes->DA_Resource = Res.DA_Resource;
		SpawnedRes->ResourceSize = Res.ResourceSize;
		SpawnedRes->SM_Variety = Res.SM_Variety;
		SpawnedRes->Growing = Res.Growing;
		SpawnedRes->GrowMarkTime = Res.GrowMarkTime;
		SpawnedRes->GrowSavedTime = Res.GrowSavedTime;
		SpawnedRes->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepRelativeTransform);
		SpawnedRes->FinishSpawning(ResTransform);
		SpawnedLOD->Resources.Add(SpawnedRes);
	}
}
void UEntityGenerator::LoadNPCs(TArray<FSS_NPC> NPCs, int32 LOD)
{
	if (NPCs.IsEmpty()) return;
	if (!IsValid(GetOwner())) return;
	AIsland* Island = Cast<AIsland>(GetOwner());
	FEntities* SpawnedLOD = SpawnedLODs.Find(LOD);
	if (!SpawnedLOD) SpawnedLOD = &SpawnedLODs.Add(LOD, FEntities{});
	for (const FSS_NPC npc : NPCs)
	{
		if (!IsValid(npc.NPC_Class)) return;
		FTransform LoadTransform = npc.Transform;
		LoadTransform.SetLocation(npc.Transform.GetLocation() + FVector(0,0,100));
		ANPC* SpawnedNPC = GetWorld()->SpawnActorDeferred<ANPC>(npc.NPC_Class, LoadTransform);
		SpawnedNPC->Island = Island;
		SpawnedNPC->FinishSpawning(LoadTransform);
		SpawnedNPC->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepRelativeTransform);
		SpawnedNPC->HealthSystem->Health = npc.Health;
		SpawnedNPC->LoadNPC(npc);
		SpawnedLOD->NPCs.Add(SpawnedNPC);
	}
}
bool UEntityGenerator::LoadLOD(TArray<FSS_IslandLOD> SS_LODs, int32 LoadLOD)
{
	for (const FSS_IslandLOD SS_LOD : SS_LODs)
	{
		if (SS_LOD.LOD != LoadLOD) continue;
		LoadResources(SS_LOD.Resources, LoadLOD);
		LoadNPCs(SS_LOD.NPCs, LoadLOD);
		return true;
	}
	return false;
}
void UEntityGenerator::DestroyLODs()
{
	for (TPair<int32, FEntities> LOD : SpawnedLODs)
	{
		for (AResource* Res : LOD.Value.Resources)
		{
			if (!IsValid(Res)) return;
			Res->Destroy();
		}
		for (ANPC* NPC : LOD.Value.NPCs)
		{
			if (!IsValid(NPC)) return;
			NPC->Destroy();
		}
	}
	SpawnedLODs.Empty();
}