// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingBoxComponent.h"

FVector UBuildingBoxComponent::GetBBC_Position()
{
	return GetComponentLocation()+AdditionalOrigin;
}
