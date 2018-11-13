// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VegetationGeneratorPlugin.h"
#include "VegetationGeneratorPluginEdMode.h"





#define LOCTEXT_NAMESPACE "IVegetationGeneratorPluginModule"
const FName VegGenEditAppIdentifier = FName(TEXT("VegGenEdApp"));
void IVegetationGeneratorPluginModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FEditorModeRegistry::Get().RegisterMode<FVegetationGeneratorPluginEdMode>(FVegetationGeneratorPluginEdMode::EM_VegetationGeneratorPluginEdModeId, LOCTEXT("VegetationGeneratorPluginEdModeName", "VegetationGeneratorPluginEdMode"), FSlateIcon(), true);
}

void IVegetationGeneratorPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FEditorModeRegistry::Get().UnregisterMode(FVegetationGeneratorPluginEdMode::EM_VegetationGeneratorPluginEdModeId);
}
#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(IVegetationGeneratorPluginModule, VegetationGeneratorPlugin)