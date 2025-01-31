// ADIAN Copyrighted


#include "SkyCharacterMovementComponent.h"
#include "SkyCraft/PlayerNormal.h"

USkyCharacterMovementComponent::USkyCharacterMovementComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}

// void USkyCharacterMovementComponent::UpdateBasedMovement(float DeltaSeconds)
// {
// 	if (CharacterOwner->GetRootComponent() == nullptr || CharacterOwner->GetRootComponent()->GetAttachParent() == nullptr)
// 	{
// 		Super::UpdateBasedMovement(DeltaSeconds);
// 	}
// 	else
// 	{
// 		// GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Yellow, StaticEnum<EMovementMode>()->GetNameStringByValue(MovementMode));
// 		return;
//  	}
// }

// void USkyCharacterMovementComponent::ServerMoveHandleClientError(float ClientTimeStamp, float DeltaTime, const FVector& Accel, const FVector& RelativeClientLoc, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode)
// {
// 	if (CharacterOwner->GetRootComponent() == nullptr || CharacterOwner->GetRootComponent()->GetAttachParent() == nullptr)
// 	{
// 	 	Super::ServerMoveHandleClientError(ClientTimeStamp, DeltaTime, Accel, RelativeClientLoc, ClientMovementBase, ClientBaseBoneName, ClientMovementMode);
// 	}
// 	else
// 	{
// 		return;
// 	}
// }
