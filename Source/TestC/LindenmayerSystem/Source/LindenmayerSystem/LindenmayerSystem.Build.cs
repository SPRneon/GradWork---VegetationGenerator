// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class LindenmayerSystem : ModuleRules
{
	public LindenmayerSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"LindenmayerSystem/Source/LindenmayerSystem/Public/",
                "TestC/LindenmayerSystem/Source/LindenmayerSystem/Public/"
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"LindenmayerSystem/Source/LindenmayerSystem/Private",
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"InputCore",
				"UnrealEd",
				"LevelEditor",
                "EditorStyle",
                "InputCore",
                "Slate",
				"SlateCore",
                "DetailCustomizations",
                 "Foliage"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		
	}
}
