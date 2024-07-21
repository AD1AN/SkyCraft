// ADIAN Copyrighted

#include "AUD_StaticMeshCustomPrimitiveData.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"


void UAUD_StaticMeshCustomPrimitiveData::SearchAndPopulateCPD(UMaterialInterface* Material)
{
    // Working only with scalars. TODO: add support for Vectors parameters

    TArray<FMaterialParameterInfo> OutParameterInfo;
    TArray<FGuid> OutParameterIds;
    UMaterialInstance* MaterialInstance = Cast<UMaterialInstance>(Material);
    if (MaterialInstance)
    {
        MaterialInstance->GetAllScalarParameterInfo(OutParameterInfo, OutParameterIds);
        for (const FMaterialParameterInfo& ParameterInfo : OutParameterInfo)
        {
            FMaterialParameterMetadata ParameterMetadata;
            if (MaterialInstance->GetParameterValue(EMaterialParameterType::Scalar, FMemoryImageMaterialParameterInfo(ParameterInfo.Name), ParameterMetadata, EMaterialGetParameterValueFlags::CheckAll))
            {
                if (ParameterMetadata.PrimitiveDataIndex != INDEX_NONE)
                {
                    FCustomPrimitiveDataEntry NewEntry;
                    NewEntry.Name = ParameterInfo.Name;
                    float ScalarValue;
                    if (MaterialInstance->GetScalarParameterValue(ParameterInfo, ScalarValue))
                    {
                        NewEntry.Value = ScalarValue;
                        CPDArray.Add(NewEntry);
                    }
                }
            }
        }
    }
    
}

#if WITH_EDITOR
void UAUD_StaticMeshCustomPrimitiveData::PostInitProperties()
{
    Super::PostInitProperties();
    
    if (UStaticMesh* StaticMesh = Cast<UStaticMesh>(GetOuter()))
    {
        for (int32 i = 0; i < StaticMesh->GetStaticMaterials().Num(); ++i)
        {
            UMaterialInterface* Material = StaticMesh->GetMaterial(i);
            SearchAndPopulateCPD(Material);
        }
    }
}

void UAUD_StaticMeshCustomPrimitiveData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

    if (PropertyName == GET_MEMBER_NAME_CHECKED(UAUD_StaticMeshCustomPrimitiveData, Reset))
    {
        CPDArray.Empty();
        PostInitProperties();
        Reset = false;
    }
}
#endif