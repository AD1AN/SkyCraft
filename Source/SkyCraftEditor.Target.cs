// ADIAN Copyrighted

using UnrealBuildTool;
using System.Collections.Generic;

public class SkyCraftEditorTarget : TargetRules
{
	public SkyCraftEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		
		// Update build settings
		DefaultBuildSettings = BuildSettingsVersion.Latest;

		// Update include order version
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.AddRange( new string[] { "SkyCraft", "SkyCraftEditor" } );
	}
}
