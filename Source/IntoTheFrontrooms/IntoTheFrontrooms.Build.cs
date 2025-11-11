// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class IntoTheFrontrooms : ModuleRules
{
	public IntoTheFrontrooms(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput", 
			"UMG", 
			"Slate", 
			"SlateCore", 
			"AIModule", 
			"NavigationSystem",
			"Niagara" // Added for particle effects (UE5)
		});
	}
}
