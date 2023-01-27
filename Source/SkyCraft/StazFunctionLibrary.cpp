// Staz Lincord Copyrighted


#include "StazFunctionLibrary.h"

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
