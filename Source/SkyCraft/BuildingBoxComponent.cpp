// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingBoxComponent.h"

UBuildingBoxComponent::UBuildingBoxComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UBuildingBoxComponent::OnRegister()
{
	Super::OnRegister();
	if (ShowSnap)
	{
		CubeMeshComponent = NewObject<UStaticMeshComponent>(this, TEXT("CubeMeshComponent"));
		CubeMeshComponent->SetupAttachment(this);
		FSoftObjectPath CubeMeshAssetRef(TEXT("/Game/s2u/Cube1x1"));
		UStaticMesh* CubeMeshAsset = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, *CubeMeshAssetRef.ToString()));
		CubeMeshComponent->SetStaticMesh(CubeMeshAsset);
		
		CubeMeshComponent->SetRelativeScale3D(GetUnscaledBoxExtent() / 50.0f);
	}
}

FVector UBuildingBoxComponent::GetBBC_Position()
{
	FTransform BBCTransform = GetComponentTransform();
	return GetComponentLocation()+BBCTransform.TransformVector(AdditionalOrigin);
}

UBuildingBoxComponent* UBuildingBoxComponent::Get_BuildingBoxComponent()
{
	return this;
}
