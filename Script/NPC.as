struct FNewNPCInstance
{
	UPROPERTY()
	UDA_NewNPC DA_NPC;

	UPROPERTY()
	int32 MaxInstances = 0;

	UPROPERTY(VisibleInstanceOnly)
	float CurrentSpawnTime = 0;

	UPROPERTY(VisibleInstanceOnly)
	TArray<ANewNPC> SpawnedNPCs;
};

enum ENewNPCType
{
	Normal,
	Corrupted,
	Nocturnal
};

delegate void FOnMaterialLoaded(UObject Object, UObject LoadedObject, int32 Index);

UCLASS(Abstract)
class ANewNPC : AAdianCharacter //, IEntityInterface, IIslandInterface
{
	UPROPERTY(DefaultComponent)
	UEntityComponent EntityComponent;

	UPROPERTY(DefaultComponent)
	USuffocationComponent SuffocationComponent;

	UPROPERTY()
	ANewGSS GSS;

	UPROPERTY(VisibleInstanceOnly, Replicated)
	UDA_NewNPC DA_NPC;

	UPROPERTY(BlueprintReadWrite, Replicated, VisibleInstanceOnly)
	ANewIsland ParentIsland;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	int32 IslandLODIndex = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	ENPCType NPCType = ENPCType::Normal;

	UPROPERTY(VisibleInstanceOnly)
	float NocturnePerishTime = 0;

	UPROPERTY(VisibleInstanceOnly)
	float NocturnePerishTimeMax;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	AIslandCrystal IslandCrystal; // Used for NPC to attack.

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveFromIsland()
	{

	}

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddToIsland(AIsland NewIsland)
	{
		
	}

	UPROPERTY(Replicated)
	TSubclassOf<UNewCorruptionOverlayEffect> SpawnWithCorruptionOverlayEffect;

	UFUNCTION()
	void OnCorruptionOverlayEffectDestroyed(UActorComponent Component)
	{
		SpawnWithCorruptionOverlayEffect = nullptr;
	}

	FOnMaterialLoaded OnMaterialLoaded;

	UFUNCTION()
	void MaterialLoaded(UObject Object)
	{
		UMaterial LoadedMaterial = Cast<UMaterial>(Object);

		for (auto& OverrideMaterial : DA_NPC.OverrideMaterials)
		{
			if (OverrideMaterial.Material == LoadedMaterial)
			{
				Mesh.SetMaterial(OverrideMaterial.Index, LoadedMaterial);
			}
		}
	}

	UFUNCTION(BlueprintOverride)
	void BeginActor()
	{
		if (!IsValid(GSS)) GSS = Cast<ANewGSS>(World.GameState);

		for (auto& OverrideMaterial : DA_NPC.OverrideMaterials)
		{
			if (OverrideMaterial.Material.IsNull())
			{
				Print(f"{DA_NPC}: %s contains null override material.");
				continue;
			}

			OverrideMaterial.Material.LoadAsync(FOnSoftObjectLoaded(this, n"MaterialLoaded")); // Maybe bug here? Does LoadAsync happen if in loop?
		}

		if (NPCType == ENPCType::Corrupted)
		{
			if (IsValid(ParentIsland)) ParentIsland.AddCorruptedNPC(this);

			if (IsValid(SpawnWithCorruptionOverlayEffect))
			{

				UNewCorruptionOverlayEffect CorruptionOverlayEffect = CreateComponent(SpawnWithCorruptionOverlayEffect);
				CorruptionOverlayEffect.SkeletalMeshComponent = Mesh;
				CorruptionOverlayEffect.StartOverlay();
				if (HasAuthority()) CorruptionOverlayEffect.OnComponentDeactivated.AddUFunction(this, n"OnCorruptionOverlayEffectDestroyed");
			}
		}
		else if (NPCType == ENPCType::Nocturnal)
		{
			NocturnePerishTimeMax = Math::RandRange(5.0f, 20.0f);
		}

		if (!HasAuthority())
			return;
		SetActorTickEnabled(true);

		if (!IsValid(ParentIsland))
			return;

		UpdateSettings();

		ParentIsland.OnServerLOD.BindUFunction(this, n"ChangedLOD");
	}

	UFUNCTION(BlueprintOverride)
	void Tick(float DeltaSeconds)
	{
		
	}

	UPROPERTY(VisibleInstanceOnly)
	bool bIsActive = true;

	UFUNCTION()
	void ChangedLOD()
	{

	}

	void UpdateSettings()
	{

	}

	FOnNewBase OnNewBase;

	// virtual void SetBase(UPrimitiveComponent NewBase, const FName BoneName, bool bNotifyActor) override;

	UFUNCTION()
	FSS_NewNPC SaveNPC()
	{
		FSS_NewNPC SS_NPC;
		SS_NPC.Health = EntityComponent.GetHealth();
		SS_NPC.Transform = ActorTransform;

		return SS_NPC;
	}

	UFUNCTION(BlueprintCallable)
	bool LoadNPC(const FSS_NewNPC& SS_NPC)
	{
		EntityComponent.SetHealth(SS_NPC.Health);
		return true;
	}

	UFUNCTION()
	void DelayedDestroy()
	{

	}

	void NextFrameDestroy()
	{
		
	}

	ANewIsland GetIsland()
	{
		return ParentIsland;
	}

	// ~Begin IEntityInterface
	// virtual void NativeOnDie(const FDamageInfo& DamageInfo) override;
	// ~End IEntityInterface
};