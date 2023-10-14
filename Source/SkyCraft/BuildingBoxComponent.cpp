// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingBoxComponent.h"

UBuildingBoxComponent::UBuildingBoxComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
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
