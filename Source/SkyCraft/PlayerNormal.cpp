// ADIAN Copyrighted

#include "PlayerNormal.h"

#include "AdianFL.h"
#include "GSS.h"
#include "Island.h"
#include "PlayerIsland.h"
#include "PSS.h"
#include "Components/EntityComponent.h"
#include "Components/HealthRegenComponent.h"
#include "Components/HungerComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/SkySpringArmComponent.h"
#include "DataAssets/DA_Item.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Enums/ItemType.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "SkyCraft/Components/SkyCharacterMovementComponent.h"

APlayerNormal::APlayerNormal(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	EntityComponent = CreateDefaultSubobject<UEntityComponent>("EntityComponent");
	
	HealthRegenComponent = CreateDefaultSubobject<UHealthRegenComponent>("HealthRegenComponent");
	HungerComponent = CreateDefaultSubobject<UHungerComponent>("HungerComponent");

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");
	InventoryComponent->ComponentTags.Add("Inventory");
	InventoryComponent->Slots.AddDefaulted(40);
	
	EquipmentInventoryComponent = CreateDefaultSubobject<UInventoryComponent>("EquipmentInventoryComponent");
	EquipmentInventoryComponent->Slots.AddDefaulted(4);
	EquipmentInventoryComponent->DropInItemTypes = {EItemType::Item, EItemType::ItemComponent};

	SkySpringArmComponent = CreateDefaultSubobject<USkySpringArmComponent>("SkySpringArmComponent");
	SkySpringArmComponent->SetupAttachment(RootComponent);
	SkySpringArmComponent->SetRelativeLocation(FVector(0, 0, 55.0f));
	SkySpringArmComponent->TargetArmLength = 250.0f;
	SkySpringArmComponent->TargetArmLengthInitial = 250.0f;
	SkySpringArmComponent->SocketOffset = FVector(0, 50.0f, 0);
	SkySpringArmComponent->ProbeSize = 12.0f;
	SkySpringArmComponent->bEnableCameraLag = true;
	SkySpringArmComponent->CameraLagSpeed = 10.0f;
	SkySpringArmComponent->CameraLagMaxDistance = 100.0f;
	SkySpringArmComponent->ComponentTags.Add("MainSkySpringArm");

	SM_Head = CreateDefaultSubobject<USkeletalMeshComponent>("SM_Head");
	SM_Head->SetupAttachment(GetMesh());
	SM_Head->PrimaryComponentTick.bStartWithTickEnabled = false;
	SM_Head->ClothVelocityScale = 0.0f;
	SM_Head->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SM_Head->bUseAttachParentBound = true;
	SM_Head->bUseBoundsFromLeaderPoseComponent = true;
	SM_Head->bComponentUseFixedSkelBounds = true;
	
	SM_Outfit = CreateDefaultSubobject<USkeletalMeshComponent>("SM_Outfit");
	SM_Outfit->SetupAttachment(GetMesh());
	SM_Outfit->PrimaryComponentTick.bStartWithTickEnabled = false;
	SM_Outfit->ClothVelocityScale = 0.0f;
	SM_Outfit->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SM_Outfit->bUseAttachParentBound = true;
	SM_Outfit->bUseBoundsFromLeaderPoseComponent = true;
	SM_Outfit->bComponentUseFixedSkelBounds = true;
	
	SM_Hands = CreateDefaultSubobject<USkeletalMeshComponent>("SM_Hands");
	SM_Hands->SetupAttachment(GetMesh());
	SM_Hands->PrimaryComponentTick.bStartWithTickEnabled = false;
	SM_Hands->ClothVelocityScale = 0.0f;
	SM_Hands->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SM_Hands->bUseAttachParentBound = true;
	SM_Hands->bUseBoundsFromLeaderPoseComponent = true;
	SM_Hands->bComponentUseFixedSkelBounds = true;
	
	SM_Feet = CreateDefaultSubobject<USkeletalMeshComponent>("SM_Feet");
	SM_Feet->SetupAttachment(GetMesh());
	SM_Feet->PrimaryComponentTick.bStartWithTickEnabled = false;
	SM_Feet->ClothVelocityScale = 0.0f;
	SM_Feet->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SM_Feet->bUseAttachParentBound = true;
	SM_Feet->bUseBoundsFromLeaderPoseComponent = true;
	SM_Feet->bComponentUseFixedSkelBounds = true;
}

void APlayerNormal::BeginActor_Implementation()
{
	if (HasAuthority())
	{
		CharacterStart();
	}
}

void APlayerNormal::OnRep_PSS_Implementation()
{
	CharacterStart();
}

void APlayerNormal::CharacterStart_Implementation()
{
	if (bCharacterStarted) return;
	GSS = GetWorld()->GetGameState<AGSS>();

	InitialUpdateEquipmentSlots();
	EquipmentInventoryComponent->OnSlotItem.AddDynamic(this, &APlayerNormal::UpdateEquipmentSlot);
	
	HealthRegenComponent->ManualBeginPlay(EntityComponent);
	HungerComponent->OnHunger.AddDynamic(this, &APlayerNormal::OnHunger);
}

void APlayerNormal::Tick(float DeltaSeconds)
{
	SkySpringArmComponent->SetWorldRotation(LookRotation); // Should be always first.
	
	Super::Tick(DeltaSeconds);

	if (IsLocallyControlled()) Server_SyncLookRotation(LookRotation);
}

void APlayerNormal::Server_SyncLookRotation_Implementation(FRotator NewLookRotation)
{
	NewLookRotation.Pitch = FRotator::NormalizeAxis(NewLookRotation.Pitch);
	NewLookRotation.Yaw = FRotator::NormalizeAxis(NewLookRotation.Yaw);
	Multicast_SyncLookRotation(NewLookRotation);
}

void APlayerNormal::Multicast_SyncLookRotation_Implementation(FRotator NewLookRotation)
{
	if (IsLocallyControlled()) return;
	LookRotation = NewLookRotation;
	// It will be used in tick.
}

void APlayerNormal::Multicast_SetLookRotation_Implementation(FRotator NewLookRotation)
{
	LookRotation = NewLookRotation;
	SkySpringArmComponent->SetWorldRotation(LookRotation);
}

void APlayerNormal::InitialUpdateEquipmentSlots()
{
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

	TArray<FSlot> EquipmentSlots = EquipmentInventoryComponent->Slots;
	
	for (int32 SlotIndex = 0; SlotIndex < EquipmentSlots.Num(); ++SlotIndex)
	{
		if (UDA_Item* DA_Item = EquipmentSlots[SlotIndex].DA_Item.Get())
		{
			AddEquipmentStats(DA_Item);
			
			TSoftObjectPtr<USkeletalMesh> MeshAsset = (PSS->CharacterBio.Gender) ? DA_Item->EQ_Male : DA_Item->EQ_Female;

			if (MeshAsset.IsNull())
			{
				GetEquipmentMeshComponent(SlotIndex)->SetSkeletalMesh(nullptr);
				continue;
			}

			Streamable.RequestAsyncLoad(
				MeshAsset.ToSoftObjectPath(),
				FStreamableDelegate::CreateUObject(this,
					&APlayerNormal::OnMeshLoaded,
					MeshAsset,
					SlotIndex
				)
			);
		}
	}
}

void APlayerNormal::UpdateEquipmentSlot(int32 SlotIndex, UDA_Item* OldItem)
{
	RemoveEquipmentStats(OldItem);
	
	if (UDA_Item* NewItem = EquipmentInventoryComponent->Slots[SlotIndex].DA_Item.Get())
	{
		AddEquipmentStats(NewItem);
		
		USkeletalMesh* SkeletalMesh;
		
		if (PSS->CharacterBio.Gender) SkeletalMesh = NewItem->EQ_Male.LoadSynchronous();
		else SkeletalMesh = NewItem->EQ_Female.LoadSynchronous();
		
		if (SkeletalMesh)
		{
			GetEquipmentMeshComponent(SlotIndex)->SetSkeletalMesh(SkeletalMesh);
		}
	}
	else
	{
		GetEquipmentMeshComponent(SlotIndex)->SetSkeletalMesh(nullptr);
	}
}

void APlayerNormal::OnMeshLoaded(TSoftObjectPtr<USkeletalMesh> MeshAsset, int32 SlotIndex)
{
	USkeletalMesh* LoadedMesh = MeshAsset.Get();

	if (!LoadedMesh) return;

	GetEquipmentMeshComponent(SlotIndex)->SetSkeletalMesh(LoadedMesh);
}

void APlayerNormal::AddEquipmentStats(UDA_Item* NewItem)
{
	if (!NewItem) return;
	if (!NewItem->EquipmentStatsModifier.IsValid()) return;
	
	const FEntityStatsModifier* StatsPtr = NewItem->EquipmentStatsModifier.GetPtr<FEntityStatsModifier>();
	EntityComponent->AddStatsModifier(StatsPtr);
}

void APlayerNormal::RemoveEquipmentStats(UDA_Item* OldItem)
{
	if (!OldItem) return;
	if (!OldItem->EquipmentStatsModifier.IsValid()) return;
	
	const FEntityStatsModifier* StatsPtr = OldItem->EquipmentStatsModifier.GetPtr<FEntityStatsModifier>();
	EntityComponent->RemoveStatsModifier(StatsPtr);
}

void APlayerNormal::SetBase(UPrimitiveComponent* NewBase, const FName BoneName, bool bNotifyActor)
{
	AIsland* NewIsland = nullptr;
	
	if (NewBase)
	{
		AActor* NewBaseRoot = UAdianFL::GetRootActor(NewBase->GetOwner());
		if (NewBaseRoot && NewBaseRoot->IsA(AIsland::StaticClass()))
		{
			NewIsland = Cast<AIsland>(NewBaseRoot);
			NewBase = NewIsland->PMC_Main;
		}
	}
	
	UPrimitiveComponent* OldBase = BasedMovement.MovementBase;
	Super::SetBase(NewBase, BoneName, bNotifyActor);
	
	if (NewBase != OldBase)
	{
		REP_SET(ParentIsland, NewIsland);
		OnNewBase.Broadcast();
	}
}

void APlayerNormal::OnRep_ParentIsland_Implementation(AIsland* OldValue)
{
	if (PSS->IsLocalState())
	{
		APlayerIsland* OldPlayerIsland = Cast<APlayerIsland>(OldValue);
		
		// Unbind from old PlayerIsland.
		if (IsValid(OldPlayerIsland))
		{
			OldPlayerIsland->OnStopIsland.RemoveDynamic(this, &APlayerNormal::OnParentPlayerStopIsland);
		}

		ParentPlayerIsland = Cast<APlayerIsland>(ParentIsland);

		// Bind to new PlayerIsland.
		UpdateCameraLag();
		
		if (IsValid(ParentPlayerIsland)) ParentPlayerIsland->OnStopIsland.AddDynamic(this, &APlayerNormal::OnParentPlayerStopIsland);
	}
}

void APlayerNormal::OnHunger()
{
	if (EntityComponent->GetHealth() >= EntityComponent->HealthMax) return;
	if (HungerComponent->Hunger < HungerComponent->MaxHunger/2)
	{
		if (!HealthRegenComponent->IsComponentTickEnabled()) HealthRegenComponent->SetComponentTickEnabled(true);
	}
	else
	{
		if (HealthRegenComponent->IsComponentTickEnabled()) HealthRegenComponent->SetComponentTickEnabled(false);
	}
}

void APlayerNormal::Server_SetQSI_Implementation(bool bIsMainQSI, int32 QSI)
{
	if (bIsMainQSI)
	{
		REP_SET(MainQSI, QSI);
		StoredMainQSI = QSI;
	}
	else
	{
		REP_SET(SecondQSI, QSI);
		StoredSecondQSI = QSI;
	}
}

void APlayerNormal::Server_SetBothQSI_Implementation(int32 NewMainQSI, int32 NewSecondQSI, bool bStore)
{
	REP_SET(MainQSI, NewMainQSI);
	REP_SET(SecondQSI, NewSecondQSI);
	
	if (bStore)
	{
		StoredMainQSI = NewMainQSI;
		StoredSecondQSI = NewSecondQSI;
	}
}

void APlayerNormal::Server_SpawnIC_Implementation(bool bIsMainQSI)
{
	Server_ReceiveSpawnIC(bIsMainQSI);
}

void APlayerNormal::SetHandsFull(bool bHandsFull, AActor* Actor)
{
	REP_SET(HandsFull, bHandsFull);
	HandsFullActor = Actor;
}

void APlayerNormal::Multicast_LoadInPhantomAnim_Implementation()
{
	LoadInPhantomAnim();
}

void APlayerNormal::OnRep_AnimLoopUpperBody()
{
	SetAnimLoopUpperBody(AnimLoopUpperBody);
}

void APlayerNormal::UpdateCameraLag()
{
	if (IsValid(ParentPlayerIsland))
	{
		if (ParentPlayerIsland->bStopIsland) EnableCameraLag();
		else DisableCameraLag();
	}
	else EnableCameraLag();
}

void APlayerNormal::OnParentPlayerStopIsland()
{
	if (ParentPlayerIsland->bStopIsland) EnableCameraLag();
	else DisableCameraLag();
}

void APlayerNormal::SetAnimLoopUpperBody(UAnimSequenceBase* Sequence)
{
	if (Sequence)
	{
		AnimLoopUpperBody = Sequence;
		MARK_PROPERTY_DIRTY_FROM_NAME(APlayerNormal, AnimLoopUpperBody, this);
		bAnimLoopUpperBody = true;
	}
	else
	{
		AnimLoopUpperBody = nullptr;
		MARK_PROPERTY_DIRTY_FROM_NAME(APlayerNormal, AnimLoopUpperBody, this);
		bAnimLoopUpperBody = false;
	}
}

USkeletalMeshComponent* APlayerNormal::GetEquipmentMeshComponent(int32 SlotIndex)
{
	switch (SlotIndex)
	{
	case 0: return SM_Head.Get();
	case 1: return SM_Outfit.Get();
	case 2: return SM_Hands.Get();
	case 3: return SM_Feet.Get();
	default: return nullptr;
	}
}

int32 APlayerNormal::OverrideEssence_Implementation(int32 NewEssence)
{
	ensureAlways(PSS);
	if (!IsValid(PSS)) return 0;
	PSS->Client_ActionWarning(FText::FromStringTable(GSS->ST_Warnings, TEXT("VesselIsFull")));
	return PSS->SetEssence(NewEssence);
}

int32 APlayerNormal::FetchEssence_Implementation()
{
	ensureAlways(PSS);
	if (!IsValid(PSS)) return 0;
	return PSS->GetEssence();
}

void APlayerNormal::AddEssence_Implementation(AActor* Sender, int32 AddEssence, bool& bFullyAdded)
{
	// This code identical/similar in other Player forms.
	bFullyAdded = false;
	ensureAlways(PSS);
	if (!IsValid(PSS)) return;
	
	int32 CombinedEssence = PSS->GetEssence() + AddEssence;
	if (CombinedEssence > PSS->EssenceVessel)
	{
		if (Sender && Sender->Implements<UEssenceInterface>())
		{
			int32 SenderEssence = IEssenceInterface::Execute_FetchEssence(Sender);
			SenderEssence -= AddEssence;
		
			int32 ReturnEssence = (CombinedEssence - PSS->EssenceVessel) + SenderEssence;
			IEssenceInterface::Execute_OverrideEssence(Sender, ReturnEssence);
		}
		
		bFullyAdded = false;
		PSS->SetEssence(PSS->EssenceVessel);
		PSS->Client_ActionWarning(FText::FromStringTable(GSS->ST_Warnings, TEXT("VesselIsFull")));
	}
	else
	{
		bFullyAdded = true;
		PSS->SetEssence(PSS->GetEssence() + AddEssence);
	}
}

void APlayerNormal::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;

	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerNormal, PSS, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerNormal, ParentIsland, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerNormal, PlayerPhantom, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerNormal, MainQSI, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerNormal, SecondQSI, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerNormal, HandsFull, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerNormal, Stamina, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(APlayerNormal, AnimLoopUpperBody, Params);
}
