// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SkyCharacterMovementComponent.generated.h"

UCLASS()
class SKYCRAFT_API USkyCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_UCLASS_BODY()

	virtual void UpdateBasedMovement(float DeltaSeconds) override;
	virtual void ServerMoveHandleClientError(float ClientTimeStamp, float DeltaTime, const FVector& Accel, const FVector& RelativeClientLoc, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode) override;

	// NOT IN:
	// SmoothClientPosition, SmoothCorrection, ClientUpdatePositionAfterServerUpdate
	// ServerCheckClientError, ServerExceedsAllowablePositionError, ServerShouldUseAuthoritativePosition
	// ClientAckGoodMove_Implementation, ClientAdjustPosition_Implementation, ClientVeryShortAdjustPosition_Implementation, ClientAdjustRootMotionPosition_Implementation, ClientAdjustRootMotionSourcePosition_Implementation
	// OnClientCorrectionReceived
	// UpdateFloorFromAdjustment, ClientHandleMoveResponse, ???VerifyClientTimeStamp???
	// ???ServerMove???
	// MaybeSaveBaseLocation, SaveBaseLocation, MaybeUpdateBasedMovement
	// CallServerMove, CallServerMovePacked
	// ClientAdjustPosition, ClientVeryShortAdjustPosition, ClientAckGoodMove, OnMovementUpdated
	// ServerMoveDual, ServerMoveOld
	// MoveSmooth, UpdateProxyAcceleration
	// MoveAlongFloor, OnCharacterStuckInGeometry, MaintainHorizontalGroundVelocity
	// GetPenetrationAdjustment, ResolvePenetrationImpl, HandleImpact
	// ProcessClientTimeStampForTimeDiscrepancy, FlushServerMoves, PostProcessAvoidanceVelocity, CalcAvoidanceVelocity
};