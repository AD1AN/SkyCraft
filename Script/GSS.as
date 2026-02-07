delegate void FNewOnPlayerIslandCorruptionSettings();
class ANewGSS : AGameStateBase
{
	UPROPERTY()	AGMS GMS;
	UPROPERTY()	UGIS GIS;

	FNewOnPlayerIslandCorruptionSettings OnPlayerIslandCorruptionSettings;

	// >>>>>>>>>>>>>>>>>>>>>>>>>>> World Settings
	// Any changes should be also made in UWorldSave and in Loading/SavingWorldSettings!
	void LoadWorldSettings(UNewWorldSave WorldSave)
    {
		bUseLAN = WorldSave.bUseLAN;
		bAllowInvites = WorldSave.bAllowInvites;
		bAllowJoinViaPresence = WorldSave.bAllowJoinViaPresence;
		bAllowJoinViaPresenceFriendsOnly = WorldSave.bAllowJoinViaPresenceFriendsOnly;
		bShouldAdvertise = WorldSave.bShouldAdvertise;
		NewPlayersCasta = WorldSave.NewPlayersCasta;
		bNewPlayersCastaArchonCrystal = WorldSave.bNewPlayersCastaArchonCrystal;
		ChunkRenderRange = WorldSave.ChunkRenderRange;
		ChunkSize = WorldSave.ChunkSize;
		IslandsProbability = WorldSave.IslandsProbability;
		IslandsAltitude = WorldSave.IslandsAltitude;
		TraversalAltitude = WorldSave.TraversalAltitude;
		Suffocation = WorldSave.Suffocation;
		PlayerIslandSpawnXY = WorldSave.PlayerIslandSpawnXY;
		PlayerIslandSpawnZ = WorldSave.PlayerIslandSpawnZ;
		SkyEssenceDensity = WorldSave.SkyEssenceDensity;
		bBuildingInfiniteHeight = WorldSave.BuildingInfiniteHeight;
		GroundedMax = WorldSave.GroundedMax;
		bCheatsEnabled = WorldSave.CheatsEnabled;
		PlayerHunger = WorldSave.PlayerHunger;
		bCorruptionEventEnabled = WorldSave.PlayerIslandsCorruption;
		PlayerIslandsCorruptionTime = WorldSave.PlayerIslandsCorruptionTime;
		PlayerIslandsCorruptionScale = WorldSave.PlayerIslandsCorruptionScale;
		EssenceRequireForLevel = WorldSave.EssenceRequireForLevel;
		StaminaPerLevel = WorldSave.StaminaPerLevel;
		StrengthPerLevel = WorldSave.StrengthPerLevel;
		EssenceFlowPerLevel = WorldSave.EssenceFlowPerLevel;
		EssenceVesselPerLevel = WorldSave.EssenceVesselPerLevel;
		StaminaMaxLevel = WorldSave.StaminaMaxLevel;
		StrengthMaxLevel = WorldSave.StrengthMaxLevel;
		EssenceFlowMaxLevel = WorldSave.EssenceFlowMaxLevel;
		bRespawnNPCs = WorldSave.bRespawnNPCs;
	}

	void SaveWorldSettings(UNewWorldSave WorldSave)
	{
		WorldSave.bUseLAN = bUseLAN;
		WorldSave.bAllowInvites = bAllowInvites;
		WorldSave.bAllowJoinViaPresence = bAllowJoinViaPresence;
		WorldSave.bAllowJoinViaPresenceFriendsOnly = bAllowJoinViaPresenceFriendsOnly;
		WorldSave.bShouldAdvertise = bShouldAdvertise;
		WorldSave.NewPlayersCasta = NewPlayersCasta;
		WorldSave.bNewPlayersCastaArchonCrystal = bNewPlayersCastaArchonCrystal;
		WorldSave.ChunkRenderRange = ChunkRenderRange;
		WorldSave.ChunkSize = ChunkSize;
		WorldSave.IslandsProbability = IslandsProbability;
		WorldSave.IslandsAltitude = IslandsAltitude;
		WorldSave.TraversalAltitude = TraversalAltitude;
		WorldSave.Suffocation = Suffocation;
		WorldSave.PlayerIslandSpawnXY = PlayerIslandSpawnXY;
		WorldSave.PlayerIslandSpawnZ = PlayerIslandSpawnZ;
		WorldSave.SkyEssenceDensity = SkyEssenceDensity;
		WorldSave.BuildingInfiniteHeight = bBuildingInfiniteHeight;
		WorldSave.GroundedMax = GroundedMax;
		WorldSave.CheatsEnabled = bCheatsEnabled;
		WorldSave.PlayerHunger = PlayerHunger;
		WorldSave.PlayerIslandsCorruption = bCorruptionEventEnabled;
		WorldSave.PlayerIslandsCorruptionTime = PlayerIslandsCorruptionTime;
		WorldSave.PlayerIslandsCorruptionScale = PlayerIslandsCorruptionScale;
		WorldSave.EssenceRequireForLevel = EssenceRequireForLevel;
		WorldSave.StaminaPerLevel = StaminaPerLevel;
		WorldSave.StrengthPerLevel = StrengthPerLevel;
		WorldSave.EssenceFlowPerLevel = EssenceFlowPerLevel;
		WorldSave.EssenceVesselPerLevel = EssenceVesselPerLevel;
		WorldSave.StaminaMaxLevel = StaminaMaxLevel;
		WorldSave.StrengthMaxLevel = StrengthMaxLevel;
		WorldSave.EssenceFlowMaxLevel = EssenceFlowMaxLevel;
		WorldSave.bRespawnNPCs = bRespawnNPCs;
	}

	void ResetWorldSettings()
    {
		// UWorldSave* DefaultWorldSave = Cast<UWorldSave>(UGameplayStatics::CreateSaveGameObject(WorldSaveClass));
		// LoadWorldSettings(DefaultWorldSave);
	}

	UPROPERTY()	bool bUseLAN = false;
	UPROPERTY()	bool bAllowInvites = true;
	UPROPERTY()	bool bAllowJoinViaPresence = true;
	UPROPERTY()	bool bAllowJoinViaPresenceFriendsOnly = true;
	UPROPERTY()	bool bShouldAdvertise = true;
	UPROPERTY()	ECasta NewPlayersCasta = ECasta::Denizen;
	UPROPERTY()	bool bNewPlayersCastaArchonCrystal = true;
	UPROPERTY()	int32 ChunkRenderRange = 10;
	UPROPERTY()	float ChunkSize = 100000;
	UPROPERTY()	float IslandsProbability = 0.5f; // From 0 to 1.
	UPROPERTY()	FNewFloatMinMax IslandsAltitude = FNewFloatMinMax(90000, 100000);
	UPROPERTY(Replicated) FNewFloatMinMax TraversalAltitude = FNewFloatMinMax(70000, 110000);
	UPROPERTY()	FNewFloatMinMax Suffocation = FNewFloatMinMax(50000, 150000);
	UPROPERTY()	float PlayerIslandSpawnXY = 75000.0;
	UPROPERTY()	FNewFloatMinMax PlayerIslandSpawnZ = FNewFloatMinMax(80000, 95000);
	UPROPERTY()	float SkyEssenceDensity = 1.0f;
	UPROPERTY(Replicated) bool bBuildingInfiniteHeight = false;
	UPROPERTY(Replicated) uint8 GroundedMax = 15;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_bCheatsEnabled) bool bCheatsEnabled = false;
	UPROPERTY()	float PlayerHunger = 1.0f;
	UPROPERTY(ReplicatedUsing = OnRep_PlayerIslandCorruptionSettings) bool bCorruptionEventEnabled = true;		 // Corruption = Скверна
	UPROPERTY(ReplicatedUsing = OnRep_PlayerIslandCorruptionSettings) float PlayerIslandsCorruptionTime = 3600.0f; // Seconds
	UPROPERTY()	float PlayerIslandsCorruptionScale = 1.0f;
	UPROPERTY(Replicated) int32 EssenceRequireForLevel = 1000;
	UPROPERTY(Replicated) int32 StaminaPerLevel = 1;
	UPROPERTY(Replicated) int32 StrengthPerLevel = 1;
	UPROPERTY(Replicated) int32 EssenceFlowPerLevel = 1;
	UPROPERTY(Replicated) int32 EssenceVesselPerLevel = 3000;
	UPROPERTY(Replicated) int32 StaminaMaxLevel = 1000;
	UPROPERTY(Replicated) int32 StrengthMaxLevel = 1000;
	UPROPERTY(Replicated) int32 EssenceFlowMaxLevel = 100;
	UPROPERTY()	bool bNocturneEventEnabled = true;
	UPROPERTY()	float NocturneStartTime = 1900;
	UPROPERTY()	float NocturneEndTime = 300;
	UPROPERTY()	float NocturneIntensity = 1.0f;
	UPROPERTY()	bool bRespawnNPCs = true;
	UPROPERTY()	float TimeFallingMeteors = 10000;
	UPROPERTY()	float TimeTravelingMeteors = 10000;

	UFUNCTION(BlueprintCallable)
	void OnRep_PlayerIslandCorruptionSettings()
	{
		OnPlayerIslandCorruptionSettings.ExecuteIfBound();
	}
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<< World Settings

	UFUNCTION(BlueprintOverride)
	void BeginPlay()
    {
        
	}

	UPROPERTY(Replicated)
	FRandomStream WorldSeed;
    
	UPROPERTY(BlueprintReadOnly)
	TMap<FString, FSS_RegisteredPlayer> RegisteredPlayers;

	UFUNCTION(Client)
	void Client_ReplicateRegisteredPlayers(const TArray<FString>& Keys, const TArray<FSS_RegisteredPlayer>& Values)
    {
		AssembleRegisteredPlayers(Keys, Values);
	}

	UFUNCTION(NetMulticast)
	void Multicast_ReplicateRegisteredPlayers(const TArray<FString>& Keys, const TArray<FSS_RegisteredPlayer>& Values)
    {
		if (HasAuthority()) return; // Except server.
		AssembleRegisteredPlayers(Keys, Values);
	}

	void AssembleRegisteredPlayers(const TArray<FString>& Keys, const TArray<FSS_RegisteredPlayer>& Values)
    {
		RegisteredPlayers.Empty(Keys.Num());
		for (int32 i = 0; i < Keys.Num(); ++i)
		{
			RegisteredPlayers.Add(Keys[i], Values[i]);
		}
	}

	UFUNCTION(NetMulticast)
	void Multicast_SpawnFXAttached(FCue FX, FVector LocalLocation = FVector::ZeroVector, AActor AttachTo = nullptr, USoundAttenuation AttenuationSettings = nullptr)
    {
		if (!IsValid(AttachTo)) return;
		if (IsValid(FX.Sound)) Gameplay::SpawnSoundAttached(FX.Sound, AttachTo.GetRootComponent(), NAME_None, LocalLocation, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, false, 1, 1, 0, AttenuationSettings);
		if (IsValid(FX.Niagara))
		{
			UNiagaraComponent SpawnedNiagara;
			SpawnedNiagara = Niagara::SpawnSystemAttached(FX.Niagara, AttachTo.GetRootComponent(), NAME_None, LocalLocation, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
			if (FX.bHaveNiagaraVars)
			{
				for (FNiagaraVar NiagaraVar : FX.NiagaraVars)
				{
					if (NiagaraVar.Type == ENiagaraVarType::Float) SpawnedNiagara.SetVariableFloat(NiagaraVar.Name, NiagaraVar.Float);
					else if (NiagaraVar.Type == ENiagaraVarType::Vector) SpawnedNiagara.SetVariableVec3(NiagaraVar.Name, NiagaraVar.Vector);
					else if (NiagaraVar.Type == ENiagaraVarType::Color) SpawnedNiagara.SetVariableLinearColor(NiagaraVar.Name, NiagaraVar.Color);
					else if (NiagaraVar.Type == ENiagaraVarType::StaticMesh)
					{
						if (IsValid(NiagaraVar.StaticMesh))
						{
                            Niagara::OverrideSystemUserVariableStaticMesh(SpawnedNiagara, NiagaraVar.Name.ToString(), NiagaraVar.StaticMesh);
						}
					}
				}
			}
		}
	}

	UPROPERTY(BlueprintReadOnly, Replicated)
	APSS HostPlayer;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetHostPlayer(APSS Host)
    {
		HostPlayer = Host;
		// MARK_PROPERTY_DIRTY_FROM_NAME(AGSS, HostPlayer, this);
	}

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AuthSetTraversalAltitude(FNewFloatMinMax newTraversalAltitude)
    {
		TraversalAltitude = newTraversalAltitude;
		// MARK_PROPERTY_DIRTY_FROM_NAME(AGSS, TraversalAltitude, this);
	}

	// >>>>>>>>>>>>>>>>>>>>>>>>>>>> Blueprint Classes
	UPROPERTY(EditDefaultsOnly, Category = "Blueprint Classes") TSubclassOf<UWorldSave> WorldSaveClass;
	UPROPERTY(EditDefaultsOnly, Category = "Blueprint Classes") TSubclassOf<APlayerIsland> PlayerIslandClass;
	UPROPERTY(EditDefaultsOnly, Category = "Blueprint Classes")	TSubclassOf<AEssenceActor> EssenceActorClass;
	UPROPERTY(EditDefaultsOnly, Category = "Blueprint Classes")	TSubclassOf<ADamageNumbers> DamageNumbersClass;
	UPROPERTY(EditDefaultsOnly, Category = "Blueprint Classes")	USoundAttenuation NormalAttenuationClass;
	UPROPERTY(EditDefaultsOnly, Category = "Blueprint Classes")	USoundAttenuation BigAttenuationClass;
	UPROPERTY(EditDefaultsOnly, Category = "Blueprint Classes")	TSubclassOf<APlayerCrystal> PlayerCrystalClass;
	UPROPERTY(EditDefaultsOnly, Category = "Blueprint Classes")	TSubclassOf<APlayerNormal> PlayerNormalClass;
	UPROPERTY(EditDefaultsOnly, Category = "Blueprint Classes")	TSubclassOf<APlayerPhantom> PlayerPhantomClass;
	UPROPERTY(EditDefaultsOnly, Category = "Blueprint Classes")	TSubclassOf<APlayerDead> PlayerDeadClass;

	private UPROPERTY(EditDefaultsOnly, Category = "Blueprint Classes")	TSoftObjectPtr<UStringTable> StringTableWarnings;

	FName ST_Warnings;
	UPROPERTY(EditDefaultsOnly, Category = "Blueprint Classes")	TSubclassOf<ACorruptionSpawnPoint> CorruptionSpawnPointClass;
	UPROPERTY(EditDefaultsOnly, Category = "Blueprint Classes")	TArray<TSubclassOf<ANPC>> PlayerIslandCorruptionSurgeNPCs;
	UPROPERTY(EditDefaultsOnly, Category = "Blueprint Classes")	TArray<TSubclassOf<ANPC>> WildIslandCorruptionSurgeNPCs;
	UPROPERTY(EditDefaultsOnly, Category = "Blueprint Classes")	TArray<TSubclassOf<AActor>> CorruptionSpawnPointAvoidClasses;
	UPROPERTY(EditDefaultsOnly, Category = "Blueprint Classes")	TSubclassOf<UCorruptionOverlayEffect> CorruptionOverlayEffectClass;
	UPROPERTY(EditDefaultsOnly, Category = "Blueprint Classes")	TSubclassOf<AIslandCrystal> IslandCrystalClass;
	UPROPERTY(EditDefaultsOnly, Category = "Blueprint Classes")	TSubclassOf<APawnIslandControl> PawnIslandControl;
	UPROPERTY(EditDefaultsOnly, Category = "Blueprint Classes")	TObjectPtr<UDA_DamageAction> NocturnePerishDeathDamage;
	UPROPERTY(EditDefaultsOnly, Category = "Blueprint Classes")	TSubclassOf<ATradeCityActor> TradeCityActorClass;
	// <<<<<<<<<<<<<<<<<<<<<<<<<<< Blueprint Classes

	UPROPERTY()
	FOnConnectedPlayers OnConnectedPlayers;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ConnectedPlayers)
	TArray<APSS> ConnectedPlayers;

	UFUNCTION()
	void OnRep_ConnectedPlayers()
	{
		OnConnectedPlayers.Broadcast();
	}

	UPROPERTY()
	FOnCheatsEnabled OnCheatsEnabled;

	UFUNCTION()
	void OnRep_bCheatsEnabled()
	{
		OnCheatsEnabled.Broadcast();
	}
	UFUNCTION(BlueprintCallable)
	void Set_bCheatsEnabled(bool NewValue)
	{
		// REP_SET(bCheatsEnabled, NewValue);
	}

	UFUNCTION(BlueprintEvent)
	float GetTimeOfDay() { return 0; }
};