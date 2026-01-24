// ADIAN Copyrighted

#include "NPCSpawner.h"
#include "SkyCraft/GSS.h"
#include "SkyCraft/GMS.h"
#include "SkyCraft/Island.h"
#include "SkyCraft/NPC.h"
#include "SkyCraft/PlayerIsland.h"
#include "SkyCraft/WorldEvents.h"
#include "SkyCraft/DataAssets/DA_NPC.h"

UNPCSpawner::UNPCSpawner()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickInterval = 1;
}

void UNPCSpawner::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner()->HasAuthority())
	{
		Island = Cast<AIsland>(GetOwner());
		if (Island->bPlayerIsland)
		{
			PlayerIsland = Cast<APlayerIsland>(Island);
		}
		GSS = Island->GSS;
		GMS = Island->GSS->GMS;
		NocturneInstances = GMS->WorldEvents->NocturneDefault;
		SetComponentTickEnabled(true);
	}
}

void UNPCSpawner::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	const float VertexOffset = (Island->Resolution * Island->VertexDistance) / 2;

	// Respawn island NPCs.
	if (Island->GSS->bRespawnNPCs)
	{
		// Iterate from Lowest LOD to Last LOD. Example: From 3 to 6.
		for (int32 LOD_Index = Island->LowestServerLOD; LOD_Index < Island->GetLastLOD(); LOD_Index++)
		{
			for (auto& NPCInstance : Island->SpawnedLODs[LOD_Index].NPCInstances)
			{
				// Remove all non-valid NPCs.
				NPCInstance.SpawnedNPCs.RemoveAll([](const ANPC* NPC)
				{
					return !IsValid(NPC);
				});

				if (NPCInstance.SpawnedNPCs.Num() < NPCInstance.MaxInstances)
				{
					float PopulationRatio = 1.0f;
					if (NPCInstance.MaxInstances > 0) PopulationRatio = (float)NPCInstance.SpawnedNPCs.Num() / (float)NPCInstance.MaxInstances;
					
					// Shift ratio from 100% to 75%
					PopulationRatio = FMath::Clamp(PopulationRatio / 0.75f, 0.0f, 1.0f);
					
					// Speed goes: 2x at 0% > 1x at 75% > 1x at 100%
					float SpawnSpeed = FMath::Lerp(2.0f, 1.0f, PopulationRatio);
					
					NPCInstance.CurrentSpawnTime += GetComponentTickInterval() * SpawnSpeed;
					
					if (NPCInstance.CurrentSpawnTime >= NPCInstance.DA_NPC->SpawnTime)
					{
						int32 Attempts = 0;
						while (Attempts < 30)
						{
							// Pick a random triangle
							const int32 TriangleIndex = FMath::RandRange(0, Island->IslandData.TopTriangles.Num() / 3 - 1) * 3;
							const FVector& V0 = Island->IslandData.TopVertices[Island->IslandData.TopTriangles[TriangleIndex]];
							const FVector& V1 = Island->IslandData.TopVertices[Island->IslandData.TopTriangles[TriangleIndex + 1]];
							const FVector& V2 = Island->IslandData.TopVertices[Island->IslandData.TopTriangles[TriangleIndex + 2]];
							FVector RandomPoint = Island->RandomPointInTriangle(V0, V1, V2);

							// Avoid Island Edge
							const int32 ClosestX = FMath::RoundToInt((RandomPoint.X + VertexOffset) / Island->VertexDistance);
							const int32 ClosestY = FMath::RoundToInt((RandomPoint.Y + VertexOffset) / Island->VertexDistance);
							if (Island->IslandData.EdgeTopVerticesMap.Contains(ClosestX * Island->Resolution + ClosestY)) 
							{
								++Attempts;
								continue;
							}

							FTransform NpcTransform(Island->GetActorLocation() + RandomPoint + FVector(0,0,60));
							ANPC* RespawnedNPC = GetWorld()->SpawnActorDeferred<ANPC>(NPCInstance.DA_NPC->NPCClass, NpcTransform);
							RespawnedNPC->DA_NPC = NPCInstance.DA_NPC;
							RespawnedNPC->ParentIsland = Island;
							RespawnedNPC->IslandLODIndex = LOD_Index;
							RespawnedNPC->FinishSpawning(NpcTransform);
							NPCInstance.SpawnedNPCs.Add(RespawnedNPC);
							NPCInstance.CurrentSpawnTime = 0;
							break;
						}
					}
				}
			}
		}
	}

	// Spawn Nocturnes.
	if (GSS->bNocturneEventEnabled && Island->ServerLOD == 0 && GMS->WorldEvents->bNocturneTime)
	{
		bool bCanSpawnNocturnes = true;
		
		if (IsValid(PlayerIsland) && !PlayerIsland->bStopIsland)
		{
			bCanSpawnNocturnes = false;
		}

		if (bCanSpawnNocturnes)
		{
			for (auto& Instance : NocturneInstances)
			{
				Instance.SpawnedNPCs.RemoveAll([](const ANPC* NPC)
				{
					return !IsValid(NPC);
				});
			
				if (Instance.SpawnedNPCs.Num() < Instance.MaxInstances)
				{
					float PopulationRatio = 1.0f;
					if (Instance.MaxInstances > 0) PopulationRatio = (float)Instance.SpawnedNPCs.Num() / (float)Instance.MaxInstances;
					
					// Shift ratio from 100% to 75%
					PopulationRatio = FMath::Clamp(PopulationRatio / 0.75f, 0.0f, 1.0f);
					
					// Speed goes: 2x at 0% > 1x at 75% > 1x at 100%
					float SpawnSpeed = FMath::Lerp(2.0f, 1.0f, PopulationRatio);
					
					Instance.CurrentSpawnTime += GetComponentTickInterval() * SpawnSpeed;
					
					if (Instance.CurrentSpawnTime >= Instance.DA_NPC->SpawnTime)
					{
						int32 Attempts = 0;
						while (Attempts < 30)
						{
							// Pick a random triangle
							const int32 TriangleIndex = FMath::RandRange(0, Island->IslandData.TopTriangles.Num() / 3 - 1) * 3;
							const FVector& V0 = Island->IslandData.TopVertices[Island->IslandData.TopTriangles[TriangleIndex]];
							const FVector& V1 = Island->IslandData.TopVertices[Island->IslandData.TopTriangles[TriangleIndex + 1]];
							const FVector& V2 = Island->IslandData.TopVertices[Island->IslandData.TopTriangles[TriangleIndex + 2]];
							FVector RandomPoint = Island->RandomPointInTriangle(V0, V1, V2);

							// Avoid Island Edge
							const int32 ClosestX = FMath::RoundToInt((RandomPoint.X + VertexOffset) / Island->VertexDistance);
							const int32 ClosestY = FMath::RoundToInt((RandomPoint.Y + VertexOffset) / Island->VertexDistance);
							if (Island->IslandData.EdgeTopVerticesMap.Contains(ClosestX * Island->Resolution + ClosestY)) 
							{
								++Attempts;
								continue;
							}

							FTransform NpcTransform(Island->GetActorLocation() + RandomPoint + FVector(0,0,60));
							ANPC* SpawnedNocturne = GetWorld()->SpawnActorDeferred<ANPC>(Instance.DA_NPC->NPCClass, NpcTransform);
							SpawnedNocturne->DA_NPC = Instance.DA_NPC;
							SpawnedNocturne->NPCType = ENPCType::Nocturnal;
							SpawnedNocturne->ParentIsland = Island;
							SpawnedNocturne->IslandLODIndex = 0;
							SpawnedNocturne->FinishSpawning(NpcTransform);
							Instance.SpawnedNPCs.Add(SpawnedNocturne);
							Instance.CurrentSpawnTime = 0;
							break;
						}
					}
				}
			}
		}
	}
}