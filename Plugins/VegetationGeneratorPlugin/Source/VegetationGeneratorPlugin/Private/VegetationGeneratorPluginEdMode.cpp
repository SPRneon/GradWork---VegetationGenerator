// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VegetationGeneratorPluginEdMode.h"
#include "VegetationGeneratorPluginEdModeToolkit.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"

const FEditorModeID FVegetationGeneratorPluginEdMode::EM_VegetationGeneratorPluginEdModeId = TEXT("EM_VegetationGeneratorPluginEdMode");

FVegetationGeneratorPluginEdMode::FVegetationGeneratorPluginEdMode()
{

}

FVegetationGeneratorPluginEdMode::~FVegetationGeneratorPluginEdMode()
{

}

void FVegetationGeneratorPluginEdMode::Enter()
{
	FEdMode::Enter();

	if (!Toolkit.IsValid() && UsesToolkits())
	{
		Toolkit = MakeShareable(new FVegetationGeneratorPluginEdModeToolkit);
		Toolkit->Init(Owner->GetToolkitHost());
	}
}

void FVegetationGeneratorPluginEdMode::Exit()
{
	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	// Call base Exit method to ensure proper cleanup
	FEdMode::Exit();
}

EVegGenEditingState FVegetationGeneratorPluginEdMode::GetEditingState() const
{
	UWorld* World = GetWorld();

	if (GEditor->bIsSimulatingInEditor)
	{
		return EVegGenEditingState::SIEWorld;
	}
	else if (GEditor->PlayWorld != NULL)
	{
		return EVegGenEditingState::PIEWorld;
	}
	else if (World == nullptr)
	{
		return EVegGenEditingState::Unknown;
	}

	return EVegGenEditingState::Enabled;
}

bool FVegetationGeneratorPluginEdMode::UsesToolkits() const
{
	return true;
}




