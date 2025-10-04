// ADIAN Copyrighted

#pragma once

#include "CoreMinimal.h"
#include "AdianActor.h"
#include "GameFramework/Actor.h"
#include "ChunkEssence.generated.h"

UCLASS(Blueprintable)
class SKYCRAFT_API AChunkEssence : public AAdianActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly) class AGSS* GSS = nullptr;

	virtual void BeginActor_Implementation() override;
	
	UFUNCTION(BlueprintCallable) void RandomEssence(int32& Essence, FLinearColor& EssenceColor);
};