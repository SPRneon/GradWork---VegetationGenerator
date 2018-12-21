// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class TestC : ModuleRules
{
	public TestC(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
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
            new string[] { 
                "Core", 
                "CoreUObject", 
                "Engine", 
                "InputCore", 
                "Foliage", 
                "Slate",
				"SlateCore",
                "DetailCustomizations",
                "EditorStyle",
                "GameplayTasks",
                "UMG"});

		PrivateDependencyModuleNames.AddRange(new string[] 
        { 
           "Foliage",
           "Slate", 
           "SlateCore"
            });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

       
	}
}
