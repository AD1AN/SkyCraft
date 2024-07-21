// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class SkyCraft : ModuleRules
{
	public SkyCraft(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"Niagara"
		});

		PrivateDependencyModuleNames.AddRange(new string[] 
		{ 
			"EditorStyle",            // For editor style and utilities
			"UnrealEd",               // Core editor functionality
			"AssetTools",             // For asset management
			"PropertyEditor"          // For property customization and editor extensions
		});
		
		if (Target.bBuildEditor)
		{
			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"UnrealEd",
				"LevelEditor",
				"EditorStyle",
				"AssetTools",
				"PropertyEditor"
			});
		}

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
