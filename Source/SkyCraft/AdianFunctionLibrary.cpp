// Staz Lincord Copyrighted


#include "AdianFunctionLibrary.h"

FString UStazFunctionLibrary::GetProjectVersion()
{
	FString ProjectVersion;
	GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("ProjectVersion"),
		ProjectVersion,
		GGameIni
		);
	return ProjectVersion;
}