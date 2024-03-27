// ADIAN Copyrighted


#include "ResourceGenerator.h"
#include "Math/RandomStream.h"

UResourceGenerator::UResourceGenerator()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UResourceGenerator::BeginPlay()
{
	Super::BeginPlay();
}

void UResourceGenerator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UResourceGenerator::SetupGenerator(float InAreaSize, int32 InGeneratorSeed, float InScaleRatio, float InGroundAltitude, float InLinetraceLength)
{
	this->AreaSize = InAreaSize;
	this->GeneratorSeed = InGeneratorSeed;
	this->ScaleRatio = InScaleRatio;
	this->GroundAltitude = InGroundAltitude;
	this->LinetraceLength = InLinetraceLength;
}

void UResourceGenerator::GenerateResources(int32 MaxSpawnPoints, UDA_Resource* DA_Resource, FIntMinMax ResourceSize, FIntMinMax SM_Variety)
{
	int32 _SpawnPoints = MaxSpawnPoints / (ScaleRatio - 3 - ScaleRatio);
	float _GridSize = sqrt(_SpawnPoints);
	float _CellSize = AreaSize / _GridSize;
	float _MaxOffset = _CellSize * 0.35;

	FVector ActorLoc = GetOwner()->GetActorLocation();
	UWorld* _World = GetWorld();

	FRandomStream _StreamX;
	FRandomStream _StreamY;

	float _IslandScale = FMath::Clamp(ScaleRatio, 0.25, 0.5);

	for (int32 _Row = 0; _Row < _GridSize-1; ++_Row)
	{
		
		for (int32 _Column = 0; _Column < _GridSize-1; ++_Column)
		{
			
			FVector LocalLoc = FVector(
				_StreamX.FRandRange(_MaxOffset * -1, _MaxOffset) + (_CellSize * _Column + _CellSize * 0.5) * _IslandScale,
				_StreamY.FRandRange(_MaxOffset * -1, _MaxOffset) + (_CellSize * _Row + _CellSize * 0.5) * _IslandScale,
				GroundAltitude);
			FVector WorldLoc = LocalLoc + ActorLoc;
			float _offXY = _IslandScale * (AreaSize / 2);
			FVector StartlLoc = WorldLoc - FVector(_offXY, _offXY, 0);
			FVector EndLoc = StartlLoc + LinetraceLength;
			FHitResult HitResult;
			
			if (_World->LineTraceSingleByChannel(HitResult, StartlLoc, EndLoc, ECC_Visibility))
			{
				
			}
			DrawDebugLine(_World, StartlLoc, EndLoc, FColor::Red, false, 15);
		}
	}
}
