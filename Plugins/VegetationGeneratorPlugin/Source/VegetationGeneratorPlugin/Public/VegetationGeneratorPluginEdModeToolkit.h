// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/BaseToolkit.h"
#include "Widgets/SWidget.h"
#include "Toolkits/IToolkithost.h"

class SVegGenEdit;

class FVegetationGeneratorPluginEdModeToolkit : public FModeToolkit
{
public:

	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	
	/** FModeToolkit interface */
	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost) override;

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<class SWidget> GetInlineContent() const override;

private:

	TSharedPtr<class SVegGenEdit> VegGenEdtWidget;
};
