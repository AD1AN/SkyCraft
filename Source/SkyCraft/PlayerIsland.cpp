// ADIAN Copyrighted

#include "PlayerIsland.h"
#include "CorruptionSpawnPoint.h"
#include "GMS.h"
#include "GSS.h"
#include "IslandCrystal.h"
#include "NiagaraComponent.h"
#include "PawnIslandControl.h"
#include "PSS.h"
#include "Components/FoliageHISM.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Components/TerrainChunk.h"
#include "DataAssets/DA_IslandBiome.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

#define LOCTEXT_NAMESPACE "PlayerIsland"

APlayerIsland::APlayerIsland()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bPlayerIsland = true;
	ServerLOD = 0;

	NiagaraWind = CreateDefaultSubobject<UNiagaraComponent>("NiagaraWind");
	NiagaraWind->SetupAttachment(GetRootComponent());
}

void APlayerIsland::BeginPlay()
{
	PreviousLocation = GetActorLocation();

	if (bIsCrystal) StartIsland();
	else
	{
		SpawnCliffsComponents();
	}
	
	if (HasAuthority())
	{
		AIslandCrystal* SpawnedCrystal = GetWorld()->SpawnActorDeferred<AIslandCrystal>(GSS->IslandCrystalClass, FTransform::Identity);
		SpawnedCrystal->PlayerIsland = this;
		SpawnedCrystal->FinishSpawning(FTransform::Identity);
		IslandCrystal = SpawnedCrystal;

		PawnIslandControl = GetWorld()->SpawnActorDeferred<APawnIslandControl>(GSS->PawnIslandControl, FTransform::Identity);
		PawnIslandControl->PlayerIsland = this;
		PawnIslandControl->FinishSpawning(FTransform::Identity);
		PawnIslandControl->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		OnRep_PawnIslandControl();
	}
	Super::BeginPlay();
}

void APlayerIsland::StartIsland()
{
	Seed.Reset();
	bIsGenerating = true;
	
	GSS = GetWorld()->GetGameState<AGSS>();
	if (!DA_IslandBiome && HasAuthority())
	{
		DA_IslandBiome = GSS->GMS->GetRandomIslandBiome(Seed);
	}
	SpawnCliffsComponents();

	const FIslandData _ID = GenerateIsland();
	InitialGenerateComplete(_ID);
}

void APlayerIsland::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Island Movement
	if (!bStopIsland)
	{
		PreviousLocation = GetActorLocation();
		
		// Should be TeleportPhysics. Otherwise, attached physics objects will fly away.
		ETeleportType IslandMoveType = ETeleportType::TeleportPhysics;
		FVector IslandMovement = CurrentDirection * (CurrentSpeed * 50);
		IslandMovement.Z = (AltitudeDirection * 5) + (AltitudeDirection * CurrentSpeed * 0.5);
		IslandMovement *= DeltaSeconds;
		IslandMovement += GetActorLocation();
		SetActorLocation(IslandMovement, false, nullptr, IslandMoveType);
	
		CurrentDirection = FMath::VInterpTo(CurrentDirection, TargetDirection, DeltaSeconds, 1.0f);

		float ToTargetSpeed = bStopIsland ? 0.0f : TargetSpeed;
		CurrentSpeed = FMath::FInterpTo(CurrentSpeed, ToTargetSpeed, DeltaSeconds, 0.75f);

		// Altitude movement.
		if (AltitudeDirection != 0)
		{
			bool AltitudeDirectionUp = AltitudeDirection > 0 ? GetActorLocation().Z >= TargetAltitude : GetActorLocation().Z <= TargetAltitude;
			if (AltitudeDirectionUp)
			{
				AltitudeDirection = 0.0f;
				FVector NewLocation = GetActorLocation();
				NewLocation.Z = TargetAltitude;
				SetActorLocation(NewLocation, false, nullptr, IslandMoveType);
			}
		}

		// Update Foliage (fixes hism bug for movement)
		for (auto& FoliageComp : FoliageComponents)
		{
			if (!IsValid(FoliageComp)) continue;
			FoliageComp->UpdateBounds();
			FoliageComp->MarkRenderStateDirty();
		}
	}

	// Island Corruption
	if (HasAuthority() && GSS->bCorruptionEventEnabled)
	{
		CorruptionTime += DeltaSeconds;
		if (CorruptionTime >= GSS->PlayerIslandsCorruptionTime)
		{
			Multicast_CueBeginCorruption();
			SetStopIsland(true);
			bCorruptionOngoing = true;
			CorruptionTime = 0.0f;

			for (auto& PSS : Denizens)
			{
				PSS->Client_GlobalWarning(LOCTEXT("CorruptionSurge", "Corruption Surge!\nHome Island at risk!"));
			}

			TArray<ACorruptionSpawnPoint*> CorruptionSpawnPoints;
			int32 SpawnPoints = FMath::RoundToInt((1 + IslandSize) * 10 * GSS->PlayerIslandsCorruptionScale);
			for (int32 i = 0; i < SpawnPoints; i++)
			{
				FTransform SpawnTransform;
				int32 Attempts = 0;
				while (Attempts < 50)
				{
					// Pick a random triangle
					const int32 TriangleIndex = Seed.RandRange(0, IslandData.TopTriangles.Num() / 3 - 1) * 3;
					const FVector& V0 = IslandData.TopVertices[IslandData.TopTriangles[TriangleIndex]];
					const FVector& V1 = IslandData.TopVertices[IslandData.TopTriangles[TriangleIndex + 1]];
					const FVector& V2 = IslandData.TopVertices[IslandData.TopTriangles[TriangleIndex + 2]];

					FVector RandomPoint = RandomPointInTriangle(V0, V1, V2);
					
					// Avoid Island Edge
					const int32 ClosestX = FMath::RoundToInt((RandomPoint.X + (Resolution * CellSize) / 2) / CellSize);
					const int32 ClosestY = FMath::RoundToInt((RandomPoint.Y + (Resolution * CellSize) / 2) / CellSize);
					if (IslandData.EdgeTopVerticesMap.Contains(ClosestX * Resolution + ClosestY)) 
					{
						++Attempts;
						continue;
					}

					TArray<FHitResult> HitResults;
					FCollisionQueryParams QueryParams;
					QueryParams.AddIgnoredActor(this);

					GetWorld()->SweepMultiByChannel(
						HitResults,
						GetActorLocation() + RandomPoint + FVector(0,0,40.0f),
						GetActorLocation() + RandomPoint + FVector(0,0,40.0f),
						FQuat::Identity,
						ECC_Visibility,
						FCollisionShape::MakeSphere(100.0f),
						QueryParams
					);

					bool bHitObstacle = false;
					
					for (auto& Hit : HitResults)
					{
						for (auto& Class : GSS->CorruptionSpawnPointAvoidClasses)
						{
							if (Hit.GetActor()->IsA(Class))
							{
								bHitObstacle = true;
								break;
							}
						}
						if (bHitObstacle) break;
					}
					if (bHitObstacle)
					{
						++Attempts;
						continue;
					}

					// Shouldn't be close to crystal
					if (FVector::DistSquared(RandomPoint, FVector::ZeroVector) < FMath::Square(500.0f))
					{
						++Attempts;
						continue;
					}

					bool bOverlapOtherCorruption = false;
					for (auto& Corruption : CorruptionSpawnPoints)
					{
						if (FVector::DistSquared(RandomPoint+GetActorLocation(), Corruption->GetActorLocation()) < FMath::Square(100.0f))
						{
							bOverlapOtherCorruption = true;
							break;
						}
					}
					if (bOverlapOtherCorruption)
					{
						++Attempts;
						continue;
					}
					
					// todo floor slope

					// Accept location
					SpawnTransform.SetLocation(RandomPoint);
					break;
				}
				ACorruptionSpawnPoint* SpawnPoint = GetWorld()->SpawnActorDeferred<ACorruptionSpawnPoint>(GSS->CorruptionSpawnPointClass, SpawnTransform);
				SpawnPoint->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
				SpawnPoint->AttachToIsland = this;
				SpawnPoint->ClassNPC = GSS->PlayerIslandCorruptionSurgeNPCs[FMath::RandRange(0, GSS->PlayerIslandCorruptionSurgeNPCs.Num()-1)];
				SpawnPoint->FinishSpawning(SpawnTransform);
				CorruptionSpawnPoints.Add(SpawnPoint);
				// DrawDebugSphere(GetWorld(), GetActorLocation() + SpawnTransform.GetLocation() + FVector(0,0,40.0f), 100.0f, 16, FColor::Red, false, 25.0f);
			}
		}
	}

	// Crystal attack notify
	if (CrystalAttackedNotify >= 0)
	{
		CrystalAttackedNotify -= DeltaSeconds;
	}
}

void APlayerIsland::SetIslandSize(float NewSize)
{
	PreviousIslandSize = IslandSize;
	IslandSize = NewSize;
	OnRep_IslandSize();
	MARK_PROPERTY_DIRTY_FROM_NAME(AIsland, IslandSize, this);
}

void APlayerIsland::Multicast_SetStopIsland_Implementation(bool NewStopIsland, FVector ServerLocation)
{
	REP_SET(bStopIsland, NewStopIsland);
	SetActorLocation(ServerLocation, false, nullptr, ETeleportType::TeleportPhysics); // TeleportPhysics important!
	REP_SET(CurrentSpeed, 0);
	REP_SET(AltitudeDirection, 0);
	REP_SET(TargetAltitude, ServerLocation.Z);
	CueIslandStopped();
}

void APlayerIsland::OnRep_TargetDirection()
{
	OnTargetDirection.Broadcast();
	NiagaraWind->SetVariableFloat("YawDirection", TargetDirection.ToOrientationRotator().Yaw - 90);
}

void APlayerIsland::OnRep_TargetSpeed()
{
	NiagaraWind->SetFloatParameter("TargetSpeedRatio", TargetSpeedRatio());
	OnTargetSpeed.Broadcast();
}

void APlayerIsland::OnRep_IslandSize()
{
	if (!IslandStarted) return;
	if (!bIsCrystal)
	{
		OnIslandSize.Broadcast();
		return;
	}
	
	// TODO: store Terrain EditedVertices by location and apply to new terrain.
	DestroyIslandGeometry();
	
	const FIslandData _ID = GenerateIsland();
	ResizeGenerateComplete(_ID);
	if (IslandSize < PreviousIslandSize) OnIslandShrink();
	OnIslandSize.Broadcast();
}

void APlayerIsland::ResizeGenerateComplete(const FIslandData& _ID)
{
	IslandData = _ID;
	bIDGenerated = true;
	OnIDGenerated.Broadcast();
	
	for (int32 i = 0; i < _ID.GeneratedCliffs.Num(); ++i)
	{
		CliffsComponents[i]->AddInstances(_ID.GeneratedCliffs[i].Instances, false);
	}
	if (HasAuthority())
	{
		for (int32 i = 0; i < 4; ++i)
		{
			UTerrainChunk* TerrainChunk = NewObject<UTerrainChunk>(this);
			TerrainChunk->RegisterComponent();
			TerrainChunks.Add(TerrainChunk);
		}
		SpawnFoliageComponents();
	}
	
	PMC_Main->CreateMeshSection(0, IslandData.TopVertices, IslandData.TopTriangles, IslandData.TopNormals, IslandData.TopUVs, {}, IslandData.TopTangents, true);
	PMC_Main->CreateMeshSection(1, IslandData.BottomVertices, IslandData.BottomTriangles, IslandData.BottomNormals, IslandData.BottomUVs, {}, IslandData.BottomTangents, true);

	if (DA_IslandBiome)
	{
		if (DA_IslandBiome->TopMaterial) PMC_Main->SetMaterial(0, DA_IslandBiome->TopMaterial);
		if (DA_IslandBiome->BottomMaterial) PMC_Main->SetMaterial(1, DA_IslandBiome->BottomMaterial);
	}
	
	bIsGenerating = false;
	bFullGenerated = true;
	bIsGenerating = false;
	OnFullGenerated.Broadcast();
}

void APlayerIsland::DestroyIslandGeometry()
{
	if (bIsGenerating) CancelAsyncGenerate();
	
	for (int32 i = FoliageComponents.Num() - 1; i >= 0; --i)
	{
		if (IsValid(FoliageComponents[i])) FoliageComponents[i]->DestroyComponent();
		FoliageComponents.RemoveAt(i);
	}
	
	for (int32 i = TerrainChunks.Num() - 1; i >= 0; --i)
	{
		if (IsValid(TerrainChunks[i])) TerrainChunks[i]->DestroyComponent();
	}
	
	if (bFullGenerated)
	{
		PMC_Main->ClearAllMeshSections();
		for (auto& CliffComponent : CliffsComponents)
		{
			CliffComponent->ClearInstances();
		}
		IslandData = {};
	}
}

void APlayerIsland::OnRep_bIsCrystal_Implementation()
{
	if (!IslandStarted) return;
	
	if (bIsCrystal)
	{
		const FIslandData _ID = GenerateIsland();
		ResizeGenerateComplete(_ID);
		// Changing collision updates NavMesh.
		PMC_Main->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		if (HasAuthority()) GenerateLOD(INDEX_NONE); // Generate AlwaysLOD
	}
	else
	{
		DestroyIslandGeometry();
		// Changing collision updates NavMesh.
		PMC_Main->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		DestroyLODs();
		if (HasAuthority()) SetIslandSize(0.01f);
	}
	OnIsCrystal.Broadcast();
}

void APlayerIsland::AuthAddDenizen(APSS* Denizen)
{
	if (!IsValid(Denizen)) return;
	Denizens.AddUnique(Denizen);
	MARK_PROPERTY_DIRTY_FROM_NAME(APlayerIsland, Denizens, this);
}

void APlayerIsland::AuthRemoveDenizen(APSS* Denizen)
{
	if (!IsValid(Denizen)) return;
	Denizens.RemoveSingle(Denizen);
	MARK_PROPERTY_DIRTY_FROM_NAME(APlayerIsland, Denizens, this);
}

void APlayerIsland::AuthEmptyDenizens()
{
	Denizens.Empty();
	MARK_PROPERTY_DIRTY_FROM_NAME(APlayerIsland, Denizens, this);
}

void APlayerIsland::Multicast_CueBeginCorruption_Implementation()
{
	CueBeginCorruption();
}

int32 APlayerIsland::GetIslandSizeNum()
{
	return FMath::RoundToInt(FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(1.0f, 100.0f), IslandSize));
}

void APlayerIsland::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;

	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerIsland, Essence, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerIsland, bIsCrystal, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerIsland, ArchonPSS, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerIsland, ArchonSteamID, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerIsland, Denizens, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerIsland, bStopIsland, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerIsland, TargetDirection, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerIsland, AltitudeDirection, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerIsland, TargetAltitude, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerIsland, TargetSpeed, Params);
	
	DOREPLIFETIME_CONDITION_NOTIFY(APlayerIsland, IslandCrystal, COND_InitialOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(APlayerIsland, PawnIslandControl, COND_InitialOnly, REPNOTIFY_OnChanged);

	DOREPLIFETIME(APlayerIsland, CurrentSpeed);
	DOREPLIFETIME(APlayerIsland, CurrentDirection);
	DOREPLIFETIME(APlayerIsland, CorruptionTime);
}
