// ADIAN Copyrighted

#include "AdianPlayMontageCallbackProxy.h"
#include "Animation/AnimMontage.h"
#include "AssetUserData/AUD_MontageSettings.h"
#include "Components/SkeletalMeshComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AdianPlayMontageCallbackProxy)

//////////////////////////////////////////////////////////////////////////
// UPlayMontageCallbackProxy

UAdianPlayMontageCallbackProxy::UAdianPlayMontageCallbackProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MontageInstanceID(INDEX_NONE)
	, bInterruptedCalledBeforeBlendingOut(false)
{
}

UAdianPlayMontageCallbackProxy* UAdianPlayMontageCallbackProxy::CreateProxyObjectForPlayMontage(
	class USkeletalMeshComponent* InSkeletalMeshComponent,
	class UAnimMontage* MontageToPlay,
	float PlayRate,
	float StartingPosition,
	FName StartingSection,
	bool bShouldStopAllMontages)
{
	UAdianPlayMontageCallbackProxy* Proxy = NewObject<UAdianPlayMontageCallbackProxy>();
	Proxy->SetFlags(RF_StrongRefOnFrame);
	Proxy->AdianPlayMontage(InSkeletalMeshComponent, MontageToPlay, PlayRate, StartingPosition, StartingSection, bShouldStopAllMontages);
	return Proxy;
}


bool UAdianPlayMontageCallbackProxy::AdianPlayMontage(class USkeletalMeshComponent* InSkeletalMeshComponent, 
	class UAnimMontage* MontageToPlay, 
	float PlayRate, 
	float StartingPosition, 
	FName StartingSection,
	bool bShouldStopAllMontages)
{
	bool bPlayedSuccessfully = false;
	if (InSkeletalMeshComponent)
	{
		if (UAnimInstance* AnimInstance = InSkeletalMeshComponent->GetAnimInstance())
		{
			const float MontageLength = AnimInstance->Montage_Play(MontageToPlay, PlayRate, EMontagePlayReturnType::MontageLength, StartingPosition, bShouldStopAllMontages);
			bPlayedSuccessfully = (MontageLength > 0.f);

			if (bPlayedSuccessfully)
			{
				AnimInstancePtr = AnimInstance;
				if (FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay))
				{
					MontageInstanceID = MontageInstance->GetInstanceID();
				}

				if (StartingSection != NAME_None)
				{
					AnimInstance->Montage_JumpToSection(StartingSection, MontageToPlay);
				}

				BlendingOutDelegate.BindUObject(this, &UAdianPlayMontageCallbackProxy::OnMontageBlendingOut);
				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

				MontageEndedDelegate.BindUObject(this, &UAdianPlayMontageCallbackProxy::OnMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

				AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &UAdianPlayMontageCallbackProxy::OnNotifyBeginReceived);
				AnimInstance->OnPlayMontageNotifyEnd.AddDynamic(this, &UAdianPlayMontageCallbackProxy::OnNotifyEndReceived);
			}
		}
	}

	if (!bPlayedSuccessfully)
	{
		OnInterrupted.Broadcast(NAME_None);
	}

	return bPlayedSuccessfully;
}

bool UAdianPlayMontageCallbackProxy::IsNotifyValid(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) const
{
	return ((MontageInstanceID != INDEX_NONE) && (BranchingPointNotifyPayload.MontageInstanceID == MontageInstanceID));
}

void UAdianPlayMontageCallbackProxy::OnNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	if (IsNotifyValid(NotifyName, BranchingPointNotifyPayload))
	{
		OnNotifyBegin.Broadcast(NotifyName);
	}
}

void UAdianPlayMontageCallbackProxy::OnNotifyEndReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
	if (IsNotifyValid(NotifyName, BranchingPointNotifyPayload))
	{
		OnNotifyEnd.Broadcast(NotifyName);
	}
}

void UAdianPlayMontageCallbackProxy::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	UAUD_MontageSettings* MontageSettings = Montage->GetAssetUserData<UAUD_MontageSettings>();
	if (MontageSettings && MontageSettings->bEndOnBlendOut)
	{
		if (!bInterrupted)
		{
			OnBlendOut.Broadcast(NAME_None);
		}
		else
		{
			OnInterrupted.Broadcast(NAME_None);
		}
		
		UnbindDelegates();
	}
	else
	{
		if (bInterrupted)
		{
			OnInterrupted.Broadcast(NAME_None);
			bInterruptedCalledBeforeBlendingOut = true;
		}
		else
		{
			OnBlendOut.Broadcast(NAME_None);
		}
	}
}

void UAdianPlayMontageCallbackProxy::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UAUD_MontageSettings* MontageSettings = Montage->GetAssetUserData<UAUD_MontageSettings>();
	if (MontageSettings && MontageSettings->bEndOnBlendOut)
	{
		if (!bInterrupted) OnCompleted.Broadcast(NAME_None);
		return;
	}
	
	if (!bInterrupted)
	{
		OnCompleted.Broadcast(NAME_None);
	}
	else if (!bInterruptedCalledBeforeBlendingOut)
	{
		OnInterrupted.Broadcast(NAME_None);
	}
	
	UnbindDelegates();
}

void UAdianPlayMontageCallbackProxy::UnbindDelegates()
{
	if (UAnimInstance* AnimInstance = AnimInstancePtr.Get())
	{
		AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &UAdianPlayMontageCallbackProxy::OnNotifyBeginReceived);
		AnimInstance->OnPlayMontageNotifyEnd.RemoveDynamic(this, &UAdianPlayMontageCallbackProxy::OnNotifyEndReceived);
	}
}

void UAdianPlayMontageCallbackProxy::BeginDestroy()
{
	UnbindDelegates();

	Super::BeginDestroy();
}
