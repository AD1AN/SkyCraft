// ADIAN Copyrighted

#include "RepHelpers.h"
#include "Engine/ActorChannel.h"
#include "Net/Core/PushModel/PushModel.h"

void RepDirty_Internal(UObject* Object, uint64 NetPushID, int32 RepIndex)
{
    AActor* Actor = Cast<AActor>(Object);
    if (Actor == nullptr)
    {
        Actor = Object->GetTypedOuter<AActor>();

        if (Actor == nullptr)
        {
            return;
        }
    }

    if (!Actor->HasAuthority())
    {
        return;
    }

    // Assume extensive use of dormancy, as it should be
    Actor->FlushNetDormancy();

#if WITH_PUSH_MODEL
    UEPushModelPrivate::FNetPushObjectId const TypedNetPushID(NetPushID);
    UEPushModelPrivate::MarkPropertyDirty(Object, TypedNetPushID, RepIndex);
#endif
}

FName RepNotify_GetFunctionName(UClass* Class, int32 RepIndex)
{
    // Single player games won't have rep data, this is a small evil for *The Greater Good*
    Class->SetUpRuntimeReplicationData();

    return Class->ClassReps[RepIndex].Property->RepNotifyFunc;
}

void ReplicateImmediately(AActor* Actor)
{
    if (UNetDriver* const Driver = Actor->GetNetDriver())
    {
        for (UNetConnection* const Connection : Driver->ClientConnections)
        {
            UActorChannel* Channel = Connection->FindActorChannelRef(Actor);
            if (Channel == nullptr && !Actor->IsPendingKillPending() && Driver->IsLevelInitializedForActor(Actor, Connection))
            {
                // Some actors might get immediately destroyed before they have the chance to replicate, so create the channel for them if called in BeginPlay
                Channel = Cast<UActorChannel>(Connection->CreateChannelByName(NAME_Actor, EChannelCreateFlags::OpenedLocally));
                if (Channel != nullptr)
                {
                    Channel->SetChannelActor(Actor, ESetChannelActorFlags::None);
                }
            }

            if (Channel != nullptr)
            {
                Channel->bForceCompareProperties = true;
                Channel->ReplicateActor();
                Channel->bForceCompareProperties = false;
            }
        }
    }
}

void ReplicateImmediately(UObject* Object)
{
    // TODO: Replicate just this object, not the entire actor
    ReplicateImmediately(Object->GetTypedOuter<AActor>());
}