// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EdMode.h"

class FUICommandList;

enum class EVegGenEditingState : uint8
{
	Unknown,
	Enabled,
	PIEWorld,
	SIEWorld,
};

class FVegetationGeneratorPluginEdMode : public FEdMode
{
public:
	const static FEditorModeID EM_VegetationGeneratorPluginEdModeId;
public:
	FVegetationGeneratorPluginEdMode();
	virtual ~FVegetationGeneratorPluginEdMode();

	TSharedPtr<FUICommandList> UICommandList;

	// FEdMode interface
	virtual void Enter() override;
	virtual void Exit() override;
	//virtual void Tick(FEditorViewportClient* ViewportClient, float DeltaTime) override;
	//virtual void Render(const FSceneView* View, FViewport* Viewport, FPrimitiveDrawInterface* PDI) override;
	//virtual void ActorSelectionChangeNotify() override;
	bool UsesToolkits() const override;
	// End of FEdMode interface

	EVegGenEditingState GetEditingState() const;

	/** Simgple wrapper to know if we can edit foliage based on edit state */
	bool IsEditingEnabled() const
	{
		return GetEditingState() == EVegGenEditingState::Enabled;
	}

};
