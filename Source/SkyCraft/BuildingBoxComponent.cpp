// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingBoxComponent.h"

#include "WorldCollision.h"
#include "PrimitiveViewRelevance.h"
#include "PrimitiveSceneProxy.h"
#include "SceneManagement.h"
#include "PhysicsEngine/BoxElem.h"
#include "PhysicsEngine/BodySetup.h"
#include "PrimitiveSceneProxy.h"

UBuildingBoxComponent::UBuildingBoxComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	BoxExtent = FVector(32.0f, 32.0f, 32.0f);

	bUseEditorCompositing = true;
}


void UBuildingBoxComponent::SetBoxExtent(FVector NewBoxExtent, bool bUpdateOverlaps)
{
	BoxExtent = NewBoxExtent;
	UpdateBounds();
	MarkRenderStateDirty();
	UpdateBodySetup();

	// do this if already created
	// otherwise, it hasn't been really created yet
	if (bPhysicsStateCreated)
	{
		// Update physics engine collision shapes
		BodyInstance.UpdateBodyScale(GetComponentTransform().GetScale3D(), true);

		if ( bUpdateOverlaps && IsCollisionEnabled() && GetOwner() )
		{
			UpdateOverlaps();
		}
	}
}


template <EShapeBodySetupHelper UpdateBodySetupAction, typename BodySetupType>
bool InvalidateOrUpdateBoxBodySetup(BodySetupType& ShapeBodySetup, bool bUseArchetypeBodySetup, FVector BoxExtent)
{
	check((bUseArchetypeBodySetup && UpdateBodySetupAction == EShapeBodySetupHelper::InvalidateSharingIfStale) || (!bUseArchetypeBodySetup && UpdateBodySetupAction == EShapeBodySetupHelper::UpdateBodySetup));
	check(ShapeBodySetup->AggGeom.BoxElems.Num() == 1);
	FKBoxElem* se = ShapeBodySetup->AggGeom.BoxElems.GetData();

	// @todo do we allow this now?
	// check for malformed values
	if (BoxExtent.X < KINDA_SMALL_NUMBER)
	{
		BoxExtent.X = 1.0f;
	}

	if (BoxExtent.Y < KINDA_SMALL_NUMBER)
	{
		BoxExtent.Y = 1.0f;
	}

	if (BoxExtent.Z < KINDA_SMALL_NUMBER)
	{
		BoxExtent.Z = 1.0f;
	}

	float XExtent = BoxExtent.X * 2.f;
	float YExtent = BoxExtent.Y * 2.f;
	float ZExtent = BoxExtent.Z * 2.f;

	if (UpdateBodySetupAction == EShapeBodySetupHelper::UpdateBodySetup)
	{
		// now set the PhysX data values
		se->SetTransform(FTransform::Identity);
		se->X = XExtent;
		se->Y = YExtent;
		se->Z = ZExtent;
	}
	else if(se->X != XExtent || se->Y != YExtent || se->Z != ZExtent)
	{
		ShapeBodySetup = nullptr;
		bUseArchetypeBodySetup = false;
	}

	return bUseArchetypeBodySetup;
}

void UBuildingBoxComponent::UpdateBodySetup()
{
	if (PrepareSharedBodySetup<UBuildingBoxComponent>())
	{
		bUseArchetypeBodySetup = InvalidateOrUpdateBoxBodySetup<EShapeBodySetupHelper::InvalidateSharingIfStale>(ShapeBodySetup, bUseArchetypeBodySetup, BoxExtent);
	}

	CreateShapeBodySetupIfNeeded<FKBoxElem>();

	if (!bUseArchetypeBodySetup)
	{
		InvalidateOrUpdateBoxBodySetup<EShapeBodySetupHelper::UpdateBodySetup>(ShapeBodySetup, bUseArchetypeBodySetup, BoxExtent);
	}
}

bool UBuildingBoxComponent::IsZeroExtent() const
{
	return BoxExtent.IsZero();
}

FBoxSphereBounds UBuildingBoxComponent::CalcBounds(const FTransform& LocalToWorld) const 
{
	return FBoxSphereBounds( FBox( -BoxExtent, BoxExtent ) ).TransformBy(LocalToWorld);
}



FPrimitiveSceneProxy* UBuildingBoxComponent::CreateSceneProxy()
{
	/** Represents a UBoxComponent to the scene manager. */
	class FBoxSceneProxy final : public FPrimitiveSceneProxy
	{
	public:
		SIZE_T GetTypeHash() const override
		{
			static size_t UniquePointer;
			return reinterpret_cast<size_t>(&UniquePointer);
		}

		FBoxSceneProxy(const UBuildingBoxComponent* InComponent)
			:	FPrimitiveSceneProxy(InComponent)
			,	bDrawOnlyIfSelected( InComponent->bDrawOnlyIfSelected )
			,   BoxExtents( InComponent->BoxExtent )
			,	BoxColor( InComponent->ShapeColor )
			,	LineThickness( InComponent->LineThickness )
		{
			bWillEverBeLit = false;
		}

		virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
		{
			QUICK_SCOPE_CYCLE_COUNTER( STAT_BoxSceneProxy_GetDynamicMeshElements );

			const FMatrix& LocalToWorld = GetLocalToWorld();
			
			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (VisibilityMap & (1 << ViewIndex))
				{
					const FSceneView* View = Views[ViewIndex];

					const FLinearColor DrawColor = GetViewSelectionColor(BoxColor, *View, IsSelected(), IsHovered(), false, IsIndividuallySelected() );

					FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);
					DrawOrientedWireBox(PDI, LocalToWorld.GetOrigin(), LocalToWorld.GetScaledAxis( EAxis::X ), LocalToWorld.GetScaledAxis( EAxis::Y ), LocalToWorld.GetScaledAxis( EAxis::Z ), BoxExtents, DrawColor, SDPG_World, LineThickness);
				}
			}
		}

		virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
		{
			const bool bProxyVisible = !bDrawOnlyIfSelected || IsSelected();

			// Should we draw this because collision drawing is enabled, and we have collision
			const bool bShowForCollision = View->Family->EngineShowFlags.Collision && IsCollisionEnabled();

			FPrimitiveViewRelevance Result;
			Result.bDrawRelevance = (IsShown(View) && bProxyVisible) || bShowForCollision;
			Result.bDynamicRelevance = true;
			Result.bShadowRelevance = IsShadowCast(View);
			Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
			return Result;
		}
		virtual uint32 GetMemoryFootprint( void ) const override { return( sizeof( *this ) + GetAllocatedSize() ); }
		uint32 GetAllocatedSize( void ) const { return( FPrimitiveSceneProxy::GetAllocatedSize() ); }

	private:
		const uint32	bDrawOnlyIfSelected:1;
		const FVector	BoxExtents;
		const FColor	BoxColor;
		const float LineThickness;
	};

	return new FBoxSceneProxy( this );
}


FCollisionShape UBuildingBoxComponent::GetCollisionShape(float Inflation) const
{
	FVector Extent = GetScaledBoxExtent() + Inflation;
	if (Inflation < 0.f)
	{
		// Don't shrink below zero size.
		Extent = Extent.ComponentMax(FVector::ZeroVector);
	}

	return FCollisionShape::MakeBox(Extent);
}

// if (!bHiddenInGame)
// 	{
// 		CubeMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMeshComponent"));
// 		CubeMeshComponent->SetupAttachment(this);
//
// 		static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Game/s2u/Cube1x1"));
// 		CubeMeshComponent->SetStaticMesh(CubeMeshAsset.Object);
//
// 		CubeMeshComponent->SetRelativeScale3D(GetScaledBoxExtent() / 50.0f);
// 	}
// }

FVector UBuildingBoxComponent::GetBBC_Position()
{
	FTransform BBCTransform = GetComponentTransform();
	return GetComponentLocation()+BBCTransform.TransformVector(AdditionalOrigin);
}

UBuildingBoxComponent* UBuildingBoxComponent::Get_BuildingBoxComponent()
{
	return this;
}
