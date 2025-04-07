// ADIAN Copyrighted

#pragma once

#define EXPAND(x) x

#define REP_CLASS(Class, Func) \
    void Class::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const \
    { \
        Super::GetLifetimeReplicatedProps(OutLifetimeProps); \
        \
        using Self = Class; \
        \
        FDoRepLifetimeParams RepParams; \
        RepParams.bIsPushBased = true; \
        \
        [&]() \
        Func(); \
        \
        int32 BlueprintNumReps = 0; \
        if (UBlueprintGeneratedClass* const BPClass = Cast<UBlueprintGeneratedClass>(GetClass())) \
        { \
            BlueprintNumReps = BPClass->NumReplicatedProperties; \
        } \
        \
        checkf(OutLifetimeProps.Num() == (Class::StaticClass()->ClassReps.Num() + BlueprintNumReps), TEXT(#Class "::GetLifetimeReplicatedProps does not handle all replicated properties.")) \
    }

#define REP_PROPERTY_INTERNAL_1(Property) \
    DOREPLIFETIME_WITH_PARAMS(Self, Property, RepParams)

#define REP_PROPERTY_INTERNAL_2(Property, LocalCondition) \
    { \
        FDoRepLifetimeParams LocalRepParams = RepParams; \
        LocalRepParams.Condition = LocalCondition; \
        DOREPLIFETIME_WITH_PARAMS(Self, Property, LocalRepParams) \
    }

#define REP_PROPERTY_INTERNAL_3(Property, LocalCondition, LocalNotify) \
    { \
        FDoRepLifetimeParams LocalRepParams = RepParams; \
        LocalRepParams.Condition = LocalCondition; \
        LocalRepParams.RepNotifyCondition = LocalNotify; \
        DOREPLIFETIME_WITH_PARAMS(Self, Property, LocalRepParams) \
    }

#define REP_PROPERTY_INTERNAL_SELECT(_1, _2, _3, Num, ...) Num

// Params: Property, Condition [optional], Notify [optional]
#define REP_PROPERTY(...) EXPAND(REP_PROPERTY_INTERNAL_SELECT(__VA_ARGS__, REP_PROPERTY_INTERNAL_3, REP_PROPERTY_INTERNAL_2, REP_PROPERTY_INTERNAL_1)(__VA_ARGS__))

// Push ID is statically generated, must retrieve while we have our type but pass as a uint64 to prevent including PushModel.h
#if WITH_PUSH_MODEL
#define GET_PUSH_ID(Object) Object->GetNetPushId()
#else
#define GET_PUSH_ID(Object) 0
#endif

void RepDirty_Internal(UObject* Object, uint64 NetPushID, int32 RepIndex);

// Should be called BEFORE editing the property if dormant, otherwise the replication system might not have re-shadowed the initial state
// Returns a reference to the property, to streamline edits
#define REP_DIRTY(PropertyName) \
    [&]() -> auto& \
    { \
        RepDirty_Internal(this, GET_PUSH_ID(this), (int32)ThisClass::ENetFields_Private::PropertyName); \
        return this->PropertyName; \
    }()

#define REP_DIRTY_OTHER(Other, PropertyName) \
    RepDirty_Internal(Other, GET_PUSH_ID(Other), (int32)TRemovePointer<TRemoveConst<decltype(Other)>::Type>::Type::ENetFields_Private::PropertyName)

FName RepNotify_GetFunctionName(UClass* Class, int32 RepIndex);

FORCEINLINE void RepNotify_ProcessEvent(UObject* const Object, UFunction* Function)
{
    Object->ProcessEvent(Function, nullptr);
}

template <typename TParam>
FORCEINLINE void RepNotify_ProcessEvent(UObject* const Object, UFunction* Function, TParam const& Param)
{
    Object->ProcessEvent(Function, (void*)&Param);
}

#define REP_NOTIFY(PropertyName, ...) \
    if (!this->HasAnyFlags(RF_NeedPostLoad)) \
    { \
        FName const RepNotifyFuncName = RepNotify_GetFunctionName(ThisClass::StaticClass(), (int32)ThisClass::ENetFields_Private::PropertyName); \
        if (!RepNotifyFuncName.IsNone()) \
        { \
            if (UFunction* const RepNotifyFunc = ThisClass::StaticClass()->FindFunctionByName(RepNotifyFuncName)) \
            { \
                checkf(RepNotifyFunc->NumParms <= 1, TEXT("REP_NOTIFY does not support rep notifies with more than one parameter.")); \
                \
                RepNotify_ProcessEvent(this, RepNotifyFunc, __VA_ARGS__); \
            } \
        } \
    }

#define REP_SET(PropertyName, NewValue) \
    { \
        auto const EvaluatedNewValue = NewValue; \
        if (this->PropertyName != EvaluatedNewValue) \
        { \
            RepDirty_Internal(this, GET_PUSH_ID(this), (int32)ThisClass::ENetFields_Private::PropertyName); \
            \
            decltype(this->PropertyName) const OldValue = this->PropertyName; \
            this->PropertyName = EvaluatedNewValue; \
            \
            REP_NOTIFY(PropertyName, OldValue); \
        } \
    }

#define REP_APPLY(PropertyName, Operation) \
    { \
        decltype(this->PropertyName) OldValue; \
        if (!this->HasAnyFlags(RF_NeedPostLoad) && !RepNotify_GetFunctionName(ThisClass::StaticClass(), (int32)ThisClass::ENetFields_Private::PropertyName).IsNone()) \
        { \
            OldValue = this->PropertyName; \
        } \
        \
        RepDirty_Internal(this, GET_PUSH_ID(this), (int32)ThisClass::ENetFields_Private::PropertyName); \
        \
        Operation; \
        \
        REP_NOTIFY(PropertyName, OldValue); \
    }

#define REP_ADD(PropertyName, ...) \
    REP_APPLY(PropertyName, this->PropertyName.Add(__VA_ARGS__));

#define REP_INSERT(PropertyName, NewValue, NewIndex) \
    REP_APPLY(PropertyName, this->PropertyName.Insert(NewValue, NewIndex));

#define REP_REMOVE(PropertyName, OldValue) \
    REP_APPLY(PropertyName, this->PropertyName.Remove(OldValue));

#define REP_REMOVEAT(PropertyName, OldIndex) \
    REP_APPLY(PropertyName, this->PropertyName.RemoveAt(OldIndex));

#define REP_EMPTY(PropertyName) \
    REP_APPLY(PropertyName, this->PropertyName.Empty());

static void ReplicateImmediately(AActor* Actor);
static void ReplicateImmediately(UObject* Object);
