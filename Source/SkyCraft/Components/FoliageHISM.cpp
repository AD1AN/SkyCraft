// ADIAN Copyrighted

#include "FoliageHISM.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "SkyCraft/Island.h"
#include "SkyCraft/DataAssets/DA_Foliage.h"

UFoliageHISM::UFoliageHISM()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
}

void UFoliageHISM::BeginPlay()
{
	Super::BeginPlay();
	if (IsNetMode(NM_Client)) return; // Only server
	StartComponent();
}

void UFoliageHISM::OnRep_DA_Foliage() // Only clients
{
	StartComponent();
}

void UFoliageHISM::StartComponent()
{
	SetCastShadow(false);
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetCullDistance(DA_Foliage->DrawDistance);
	InstanceStartCullDistance = DA_Foliage->CullingDistance;
	InstanceEndCullDistance = DA_Foliage->CullingDistance;
	WorldPositionOffsetDisableDistance = DA_Foliage->WPO_DisableDistance;
	
	Island = Cast<AIsland>(GetOwner());
	Island->OnIDGenerated.RemoveDynamic(this, &UFoliageHISM::StartComponent);
	if (!Island->bIDGenerated)
	{
		Island->OnIDGenerated.AddDynamic(this, &UFoliageHISM::StartComponent);
		return;
	}
	Island->FoliageComponents.AddUnique(this);
	Generate_InitialInstances(Island->IslandData);
	AddInstances(InitialInstances, false,false,false);
	if (Island->bLoadFromSave)
	{
		for (FSS_Foliage& SS_Foliage : Island->SS_Island.Foliage)
		{
			if (SS_Foliage.DA_Foliage == DA_Foliage)
			{
				LoadFromSave(SS_Foliage);
				break;
			}
		}
	}
	bComponentStarted = true;
	OnComponentStarted.Broadcast();
}

void UFoliageHISM::Generate_InitialInstances(const FIslandData& _ID)
{
	const float VertexOffset = (Island->Resolution * Island->VertexDistance) / 2;
    const float SpacingSqr = FMath::Square(DA_Foliage->Spacing);
    int32 InstanceIndex = 0;
    int32 Attempts = 0;
    while (Attempts < DA_Foliage->MaxAttempts)
    {
        // Pick a random triangle
        const int32 TriangleIndex = Island->Seed.RandRange(0, _ID.TopTriangles.Num() / 3 - 1) * 3;
        const FVector& V0 = _ID.TopVertices[_ID.TopTriangles[TriangleIndex]];
        const FVector& V1 = _ID.TopVertices[_ID.TopTriangles[TriangleIndex + 1]];
        const FVector& V2 = _ID.TopVertices[_ID.TopTriangles[TriangleIndex + 2]];

        // Generate a random point in the triangle
        FVector Candidate = Island->RandomPointInTriangle(V0, V1, V2);

        // Check if Edge
        const int32 ClosestX = FMath::RoundToInt((Candidate.X + VertexOffset) / Island->VertexDistance);
        const int32 ClosestY = FMath::RoundToInt((Candidate.Y + VertexOffset) / Island->VertexDistance);
        if (_ID.EdgeTopVerticesMap.Contains(ClosestX * Island->Resolution + ClosestY)) 
        {
        	++Attempts;
        	continue;
        }

        // Convert the point to a spatial grid key
        const int32 GridX = FMath::RoundToInt(Candidate.X / DA_Foliage->Spacing);
        const int32 GridY = FMath::RoundToInt(Candidate.Y / DA_Foliage->Spacing);
        const int32 GridKey = HashCombine(GetTypeHash(GridX), GetTypeHash(GridY));
    	if (GridMap.Contains(GridKey))
    	{
    		++Attempts;
    		continue;
    	}

        // Check neighboring grid cells for spacing
        bool bTooClose = false;
        for (int32 NeighborX = -1; NeighborX <= 1; ++NeighborX)
        {
        	for (int32 NeighborY = -1; NeighborY <= 1; ++NeighborY)
        	{
        		const int32 NeighborKey = HashCombine(GetTypeHash(GridX + NeighborX), GetTypeHash(GridY + NeighborY));
        		if (GridMap.Contains(NeighborKey) && FVector::DistSquared(GridMap[NeighborKey].Location, Candidate) < SpacingSqr)
        		{
        			bTooClose = true;
        			break;
        		}
        	}
        	if (bTooClose) break;
        }
        if (bTooClose)
        {
        	++Attempts;
        	continue;
        }
        
        const FVector TriangleNormal = FVector::CrossProduct(V2 - V0, V1 - V0).GetSafeNormal();
        if (DA_Foliage->bMaxFloorSlope)
        {
        	const float SlopeAngle = FMath::Acos(FVector::DotProduct(TriangleNormal, FVector::UpVector)) * (180.0f / PI);
        	if (SlopeAngle > DA_Foliage->MaxFloorSlope)
        	{
        		++Attempts;
        		continue;
        	}
        }

        // Accept candidate
        FTransform InitialInstance(Candidate);
        FQuat GrassRotation(FRotator::ZeroRotator);
        if (DA_Foliage->bRotationAlignGround) GrassRotation = FQuat::FindBetweenNormals(FVector::UpVector, TriangleNormal);
        const FQuat GrassYaw = FQuat(FVector::UpVector, FMath::DegreesToRadians(Island->Seed.FRandRange(0.0f, 360.0f)));
        InitialInstance.SetRotation(GrassRotation * GrassYaw);
        if (DA_Foliage->bRandomScale) InitialInstance.SetScale3D(FVector(1, 1, Island->Seed.FRandRange(DA_Foliage->ScaleZ.Min, DA_Foliage->ScaleZ.Max)));
        
        GridMap.Add(GridKey, FGridValue(InstanceIndex, Candidate, false));
        InitialInstances.Add(InitialInstance);
    	
        ++InstanceIndex;
        Attempts = 0;
    }
}

void UFoliageHISM::OnRep_InitialInstancesRemoved()
{
	if (!bComponentStarted)
	{
		OnComponentStarted.AddDynamic(this, &UFoliageHISM::OnRep_InitialInstancesRemoved);
		return;
	}
	
	TArray<int32> HideInstances;
	TArray<int32> ShowInstances;
	
	// Use sets for efficient lookups
	const TSet<int32> CurrentRemovedSet(InitialInstancesRemoved);
	const TSet<int32> PreviousRemovedSet(Previous_InitialInstancesRemoved);

	// Find DELETED instances.
	for (const int32& InitialIndex : InitialInstancesRemoved)
	{
		if (!PreviousRemovedSet.Contains(InitialIndex))
		{
			HideInstances.Add(InitialIndex);
		}
	}

	// Determine instances to show (in previous but not in current)
	for (const int32& PreviousIndex : Previous_InitialInstancesRemoved)
	{
		if (!CurrentRemovedSet.Contains(PreviousIndex))
		{
			ShowInstances.Add(PreviousIndex);
		}
	}

	for (const int32& HideIndex : HideInstances)
	{
		if (!InitialInstances.IsValidIndex(HideIndex)) continue;
		FTransform TransformHide(InitialInstances[HideIndex]);
		TransformHide.SetLocation(FVector(0,0,-20));
		TransformHide.SetScale3D(FVector(0, 0, 0));
		UpdateInstanceTransform(HideIndex, TransformHide, false);
	}
	
	for (const int32& ShowIndex : ShowInstances)
	{
		if (!InitialInstances.IsValidIndex(ShowIndex)) continue;
		UpdateInstanceTransform(ShowIndex, InitialInstances[ShowIndex], false);
	}
	
	Previous_InitialInstancesRemoved = InitialInstancesRemoved;
}

void UFoliageHISM::OnRep_DynamicInstancesAdded()
{
	if (!bComponentStarted)
	{
		OnComponentStarted.AddDynamic(this, &UFoliageHISM::OnRep_DynamicInstancesAdded);
		return;
	}
	
	const TSet CurrentDynamicInstancesSet(DynamicInstancesAdded);

	// Find REMOVED instances
	for (int32 i = Previous_DynamicInstancesAdded.Num() - 1; i >= 0; --i)
	{
		if (!CurrentDynamicInstancesSet.Contains(Previous_DynamicInstancesAdded[i]))
		{
			const int32 DynamicIndex = InitialInstances.Num() + i;
			RemoveInstance(DynamicIndex);
			Previous_DynamicInstancesAdded.RemoveAtSwap(i);
		}
	}
	
	const TSet PreviousDynamicInstancesSet(Previous_DynamicInstancesAdded);

	// Find ADDED instances
	TArray<FTransform> NewDynamicInstances;
	for (int32 i = 0; i < DynamicInstancesAdded.Num(); ++i)
	{
		FDynamicInstance& NewDynamicInstance = DynamicInstancesAdded[i];
		if (!PreviousDynamicInstancesSet.Contains(NewDynamicInstance))
		{
			NewDynamicInstances.Add(NewDynamicInstance.ToTransform());
			Previous_DynamicInstancesAdded.Add(NewDynamicInstance);
		}
	}
	if (!NewDynamicInstances.IsEmpty())
	{
		AddInstances(NewDynamicInstances, false, false,false);
	}

	for (int32 i = 0; i < DynamicInstancesAdded.Num(); ++i)
	{
		if (DynamicInstancesAdded[i].Location != Previous_DynamicInstancesAdded[i].Location)
		{
			UpdateInstanceTransform(InitialInstances.Num()+i, DynamicInstancesAdded[i].ToTransform(), false, true, true);
		}
	}
	
	Previous_DynamicInstancesAdded = DynamicInstancesAdded;
}

void UFoliageHISM::RemoveInSphere(FVector_NetQuantize Location, float Radius)
{
	const float RadiusSqr = FMath::Square(Radius);
    const float FoliageCellSize = DA_Foliage->Spacing;

    // Calculate grid bounds based on the radius
    const int32 MinGridX = FMath::RoundToInt((Location.X - Radius) / FoliageCellSize);
    const int32 MaxGridX = FMath::RoundToInt((Location.X + Radius) / FoliageCellSize);
    const int32 MinGridY = FMath::RoundToInt((Location.Y - Radius) / FoliageCellSize);
    const int32 MaxGridY = FMath::RoundToInt((Location.Y + Radius) / FoliageCellSize);

    // Iterate through grid cells within the radius
    for (int32 GridX = MinGridX; GridX <= MaxGridX; ++GridX)
    {
        for (int32 GridY = MinGridY; GridY <= MaxGridY; ++GridY)
        {
            const int32 GridKey = HashCombine(GetTypeHash(GridX), GetTypeHash(GridY));
            if (const FGridValue* GridValue = GridMap.Find(GridKey))
            {
            	int32 InstanceIndex = GridMap[GridKey].Index;
            	const FVector_NetQuantize InstanceLocation = GridMap[GridKey].Location;

            	if (FVector_NetQuantize::DistSquared(InstanceLocation, Location) <= RadiusSqr)
            	{
            		if (GridValue->bDynamicInstance)
            		{
            			// Find Index by Location
            			InstanceIndex = DynamicInstancesAdded.IndexOfByPredicate([&](const FDynamicInstance& Instance)
						{
							return Instance.Location == InstanceLocation;
						});
            			GridMap.Remove(GridKey);
            			RemoveInstance(InitialInstances.Num() + InstanceIndex);
            			DynamicInstancesAdded.RemoveAtSwap(InstanceIndex);
            			MARK_PROPERTY_DIRTY_FROM_NAME(UFoliageHISM, DynamicInstancesAdded, this);
            		}
            		else
            		{
            			FTransform HideTransform(InitialInstances[InstanceIndex]);
            			HideTransform.SetLocation(FVector(0,0,-200));
            			HideTransform.SetScale3D(FVector(0, 0, 0));
            			UpdateInstanceTransform(InstanceIndex, HideTransform, false, false, true);
            			GridMap.Remove(GridKey);
            			InitialInstancesRemoved.Add(InstanceIndex);
            			MARK_PROPERTY_DIRTY_FROM_NAME(UFoliageHISM, InitialInstancesRemoved, this);
            		}
            	}
            }
        }
    }
	GetOwner()->ForceNetUpdate();
}

void UFoliageHISM::AddInSphere(FVector_NetQuantize Location, float Radius)
{
	const float SpacingSqr = FMath::Square(DA_Foliage->Spacing);
    int32 Attempts = 0;
    while (Attempts < DA_Foliage->MaxAttempts)
    {
        // Generate random DynamicInstance point within the radius
        FVector_NetQuantize RandomDynamicInstanceLocation(
            FMath::RandRange(Location.X - Radius, Location.X + Radius),
            FMath::RandRange(Location.Y - Radius, Location.Y + Radius),
            0);
  
        // Map the candidate point to the grid
        const int32 GridX = FMath::RoundToInt(RandomDynamicInstanceLocation.X / DA_Foliage->Spacing);
        const int32 GridY = FMath::RoundToInt(RandomDynamicInstanceLocation.Y / DA_Foliage->Spacing);
    	const int32 GridKey = HashCombine(GetTypeHash(GridX), GetTypeHash(GridY));

    	if (GridMap.Contains(GridKey))
    	{
    		++Attempts;
    		continue;
    	}

        // Check neighboring grid cells for spacing
        bool bTooClose = false;
        for (int32 NeighborX = -1; NeighborX <= 1; ++NeighborX)
        {
            for (int32 NeighborY = -1; NeighborY <= 1; ++NeighborY)
            {
                const int32 NeighborKey = HashCombine(GetTypeHash(GridX + NeighborX), GetTypeHash(GridY + NeighborY));
                if (GridMap.Contains(NeighborKey) && FVector_NetQuantize::DistSquared(GridMap[NeighborKey].Location, RandomDynamicInstanceLocation) < SpacingSqr)
                {
                    bTooClose = true;
                    break;
                }
            }
            if (bTooClose) break;
        }
        if (bTooClose)
        {
            ++Attempts;
            continue;
        }

        // Find 3 closest Vertices
        TArray<TPair<int32, float>> ClosestVertices; // Pair of VertexIndex and DistanceSquared
        for (const TPair<int32, FVertexData>& VertexEntry : Island->IslandData.TopVerticesMap)
        {
            FVector VertexPosition = Island->IslandData.TopVertices[VertexEntry.Value.VertexIndex];
            float DistanceSquared = FVector2D::DistSquared(
                FVector2D(VertexPosition.X, VertexPosition.Y),
                FVector2D(RandomDynamicInstanceLocation.X, RandomDynamicInstanceLocation.Y));
  
            // Maintain a sorted list of three closest vertices
            ClosestVertices.Add(TPair<int32, float>(VertexEntry.Value.VertexIndex, DistanceSquared));
            ClosestVertices.Sort([](const TPair<int32, float>& A, const TPair<int32, float>& B)
            {
                return A.Value < B.Value;
            });
            if (ClosestVertices.Num() > 3)
            {
                ClosestVertices.Pop(); // Remove the farthest vertex if more than 3
            }
        }
  
        // Ensure we have exactly three vertices
        if (ClosestVertices.Num() < 3)
        {
            ++Attempts;
            continue;
        }

        // Retrieve the positions of the three closest vertices
        FVector V0 = Island->IslandData.TopVertices[ClosestVertices[0].Key];
        FVector V1 = Island->IslandData.TopVertices[ClosestVertices[1].Key];
        FVector V2 = Island->IslandData.TopVertices[ClosestVertices[2].Key];
  
        // Calculate Z height of the candidate based on the triangle
    	FVector BarycentricCoords = FMath::ComputeBaryCentric2D(
			FVector(RandomDynamicInstanceLocation.X, RandomDynamicInstanceLocation.Y, 0),
				FVector(V0.X, V0.Y, 0),               
				FVector(V1.X, V1.Y, 0),               
				FVector(V2.X, V2.Y, 0)              
		);
        RandomDynamicInstanceLocation.Z = BarycentricCoords.X * V0.Z + BarycentricCoords.Y * V1.Z + BarycentricCoords.Z * V2.Z;
  
        // Calculate the triangle normal and check slope limits
        FVector TriangleNormal = FVector::CrossProduct(V2 - V0, V1 - V0).GetSafeNormal();
        if (DA_Foliage->bMaxFloorSlope)
        {
            float SlopeAngle = FMath::Acos(FVector::DotProduct(TriangleNormal, FVector::UpVector)) * (180.0f / PI);
            if (SlopeAngle > DA_Foliage->MaxFloorSlope)
            {
                ++Attempts;
                continue;
            }
        }
  
        // ADD DynamicInstance
        FDynamicInstance DynamicInstance;
    	DynamicInstance.Location = RandomDynamicInstanceLocation;
        FQuat GrassRotation(FRotator::ZeroRotator);
        if (DA_Foliage->bRotationAlignGround)
        {
            GrassRotation = FQuat::FindBetweenNormals(FVector::UpVector, TriangleNormal);
        }
        const FQuat GrassYaw = FQuat(FVector::UpVector, FMath::DegreesToRadians(Island->Seed.FRandRange(0.0f, 360.0f)));
    	const FRotator GrassRotator = (GrassRotation * GrassYaw).Rotator();
        DynamicInstance.Rotation = FVector_NetQuantize(GrassRotator.Pitch, GrassRotator.Yaw, GrassRotator.Roll);
        if (DA_Foliage->bRandomScale)
        {
            DynamicInstance.Scale.Z = FMath::FRandRange(DA_Foliage->ScaleZ.Min, DA_Foliage->ScaleZ.Max);
        }
        GridMap.Add(GridKey, FGridValue(0, RandomDynamicInstanceLocation, true));
    	AddInstance(DynamicInstance.ToTransform());
    	
    	DynamicInstancesAdded.Add(DynamicInstance);
    	MARK_PROPERTY_DIRTY_FROM_NAME(UFoliageHISM, DynamicInstancesAdded, this);
    	
        Attempts = 0;
    }
	GetOwner()->ForceNetUpdate();
}

void UFoliageHISM::LoadFromSave(const FSS_Foliage& SS_Foliage)
{
	InitialInstancesRemoved = SS_Foliage.InitialInstancesRemoved;
	DynamicInstancesAdded = SS_Foliage.DynamicInstancesAdded;

	// Hide Initial instances.
	for (const int32& InstanceIndex : InitialInstancesRemoved)
	{
		if (!InitialInstances.IsValidIndex(InstanceIndex)) continue;
		const int32 GridX = FMath::RoundToInt(InitialInstances[InstanceIndex].GetLocation().X / DA_Foliage->Spacing);
		const int32 GridY = FMath::RoundToInt(InitialInstances[InstanceIndex].GetLocation().Y / DA_Foliage->Spacing);
		const int32 GridKey = HashCombine(GetTypeHash(GridX), GetTypeHash(GridY));
		GridMap.Remove(GridKey);
		FTransform HideTransform(InitialInstances[InstanceIndex]);
		HideTransform.SetLocation(FVector(0,0,-200));
		HideTransform.SetScale3D(FVector(0, 0, 0));
		UpdateInstanceTransform(InstanceIndex, HideTransform, false, false, true);
	}

	// Add Dynamic instances.
	if (DynamicInstancesAdded.IsEmpty()) return;
	TArray<FTransform> NewDynamicInstances;
	NewDynamicInstances.Reserve(DynamicInstancesAdded.Num());
	for (FDynamicInstance& DynamicInstance : DynamicInstancesAdded)
	{
		const int32 GridX = FMath::RoundToInt(DynamicInstance.Location.X / DA_Foliage->Spacing);
		const int32 GridY = FMath::RoundToInt(DynamicInstance.Location.Y / DA_Foliage->Spacing);
		const int32 GridKey = HashCombine(GetTypeHash(GridX), GetTypeHash(GridY));
		GridMap.Add(GridKey, FGridValue(0, DynamicInstance.Location, true));
		NewDynamicInstances.Add(DynamicInstance.ToTransform());
	}
	if (!NewDynamicInstances.IsEmpty())
	{
		AddInstances(NewDynamicInstances, false, false, false);
	}
}

void UFoliageHISM::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(UFoliageHISM, DA_Foliage, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(UFoliageHISM, InitialInstancesRemoved, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(UFoliageHISM, DynamicInstancesAdded, Params);
}
