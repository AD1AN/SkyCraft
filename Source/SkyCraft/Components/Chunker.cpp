// ADIAN Copyrighted

#pragma once

#include "Chunker.h"

#include "SkyCraft/ChunkIsland.h"
#include "SkyCraft/GIS.h"
#include "SkyCraft/GMS.h"
#include "SkyCraft/GSS.h"
#include "SkyCraft/Island.h"
#include "SkyCraft/DataAssets/DA_IslandBiome.h"

UChunker::UChunker()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickInterval = 1;
}

void UChunker::BeginPlay()
{
	Super::BeginPlay();
	if (!GetOwner()->HasAuthority()) return;
	GSS = GetWorld()->GetGameState<AGSS>();
	
#if WITH_EDITOR
	UGIS* GIS = GetOwner()->GetGameInstance<UGIS>();
	if (GIS->bTestChunkIslandRenderRange)
	{
		GSS->ChunkRenderRange = GIS->TestChunkIslandRenderRange;
	}
#endif
	
	SetComponentTickInterval(FMath::RandRange(0.9f, 1.1f));
	SetComponentTickEnabled(true);
	UpdateChunks();
}

void UChunker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateChunks();
}

void UChunker::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (!GetOwner()->HasAuthority()) return;
	for (auto RenderingChunk : RenderingChunks)
	{
		RenderingChunk->RemoveChunker(this);
	}
}

void UChunker::UpdateChunks()
{
	CurrentCoords.X = FMath::RoundToInt(GetOwner()->GetActorLocation().X / GSS->ChunkSize);
	CurrentCoords.Y = FMath::RoundToInt(GetOwner()->GetActorLocation().Y / GSS->ChunkSize);
	if (CurrentCoords != PreviousCoords)
	{
		PreviousCoords = CurrentCoords;
		DestroyChunks();
		SpawnChunks();
	}
}

void UChunker::DestroyChunks()
{
	for (int32 i = RenderingCoords.Num() - 1; i >= 0; --i)
	{
		FVector2D ChunkLocation = FVector2D(RenderingCoords[i].X * GSS->ChunkSize, RenderingCoords[i].Y * GSS->ChunkSize);
		FVector2D CurrentLocation = FVector2D(CurrentCoords.X * GSS->ChunkSize, CurrentCoords.Y * GSS->ChunkSize);
		const float MaxChunkDistanceSqr = FMath::Square(GSS->ChunkRenderRange * GSS->ChunkSize);
		if (FVector2D::DistSquared(ChunkLocation, CurrentLocation) >= MaxChunkDistanceSqr)
		{
			RenderingChunks[i]->RemoveChunker(this);
			RenderingChunks.RemoveAt(i);
			RenderingCoords.RemoveAt(i);
		}
	}
}

void UChunker::SpawnChunks()
{
	for (int32 X = -GSS->ChunkRenderRange; X <= GSS->ChunkRenderRange; ++X)
	{
		for (int32 Y = -GSS->ChunkRenderRange; Y <= GSS->ChunkRenderRange; ++Y)
		{
			FCoords ChunkCoords;
			ChunkCoords.X = CurrentCoords.X + X;
			ChunkCoords.Y = CurrentCoords.Y + Y;

			float ChunkDistance = FVector2D(X * GSS->ChunkSize, Y * GSS->ChunkSize).Length();
			float MaxDistance = GSS->ChunkRenderRange * GSS->ChunkSize;
			
			if (ChunkDistance < MaxDistance)
			{
				int32 FoundChunk = RenderingCoords.Find(ChunkCoords);
				if (FoundChunk == INDEX_NONE)
				{
					FoundChunk = GSS->GMS->SpawnedChunkIslandsCoords.Find(ChunkCoords);
					if (FoundChunk == INDEX_NONE)
					{
						uint32 CombinedSeed = HashCombine(HashCombine(GetTypeHash(GSS->WorldSeed.GetInitialSeed()), GetTypeHash(ChunkCoords.X)), GetTypeHash(ChunkCoords.Y));
						FRandomStream ChunkSeed(CombinedSeed);
						FTransform ChunkTransform;
						ChunkTransform.SetLocation(FVector(ChunkCoords.X * GSS->ChunkSize, ChunkCoords.Y * GSS->ChunkSize, GSS->IslandsAltitude.Max));
						AChunkIsland* SpawnedChunk = GetWorld()->SpawnActorDeferred<AChunkIsland>(AChunkIsland::StaticClass(), ChunkTransform);
						SpawnedChunk->GMS = GSS->GMS;
						SpawnedChunk->Coords = ChunkCoords;
						SpawnedChunk->ChunkSeed = ChunkSeed;
						SpawnedChunk->Chunkers.Add(this);
						SpawnedChunk->FinishSpawning(ChunkTransform);
						
						RenderingChunks.Add(SpawnedChunk);
						RenderingCoords.Add(ChunkCoords);
						GSS->GMS->SpawnedChunkIslands.Add(SpawnedChunk);
						GSS->GMS->SpawnedChunkIslandsCoords.Add(ChunkCoords);
						
						if (ChunkSeed.FRand() <= GSS->IslandsProbability) // Decide if there's Island in this chunk.
						{
							UDA_IslandBiome* DA_IslandBiome = GSS->GMS->GetRandomIslandBiome(ChunkSeed);
							float IslandSize = ChunkSeed.FRandRange(DA_IslandBiome->IslandSize.Min, DA_IslandBiome->IslandSize.Max);
							FTransform IslandTransform;
							FVector IslandLocation = ChunkTransform.GetLocation();
							
							IslandLocation.Z = ChunkSeed.RandRange(GSS->IslandsAltitude.Min, GSS->IslandsAltitude.Max);
							IslandTransform.SetLocation(IslandLocation);
							AIsland* SpawnedIsland = GetWorld()->SpawnActorDeferred<AIsland>(AIsland::StaticClass(), IslandTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
							SpawnedIsland->GSS = GSS;
							SpawnedIsland->DA_IslandBiome = DA_IslandBiome;
							// Load island from save.
							if (FSS_Island* SS_Island = GSS->GMS->SavedIslands.Find(ChunkCoords.HashCoords()))
							{
								SpawnedIsland->bLoadFromSave = true;
								SpawnedIsland->SS_Island = *SS_Island;
							}
							SpawnedIsland->ChunkIsland = SpawnedChunk;
							SpawnedIsland->Coords = ChunkCoords;
							SpawnedIsland->Seed = ChunkSeed;
							SpawnedIsland->IslandSize = IslandSize;
							SpawnedIsland->ServerLOD = SpawnedChunk->ServerLOD;
							SpawnedIsland->FinishSpawning(IslandTransform);
							SpawnedChunk->Island = SpawnedIsland;
						}
					}
					else
					{
						RenderingCoords.Add(ChunkCoords);
						RenderingChunks.Add(GSS->GMS->SpawnedChunkIslands[FoundChunk]);
						GSS->GMS->SpawnedChunkIslands[FoundChunk]->AddChunker(this);
					}
				}
				else
				{
					RenderingChunks[FoundChunk]->UpdateLOD();
				}
			}
		}
	}
}
