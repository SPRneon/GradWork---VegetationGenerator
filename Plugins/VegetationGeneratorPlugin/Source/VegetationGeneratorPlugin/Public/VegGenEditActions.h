#pragma once

#include "CoreMinimal.h"
#include "EditorStyleSet.h"
#include "Framework/Commands/Commands.h"

class FVegGenEditCommands: public TCommands<FVegGenEditCommands>
{
public:
	FVegGenEditCommands() : TCommands<FVegGenEditCommands>
	(
		"VegGenEditMode",
		NSLOCTEXT("Contexts", "VegGenEditMode", "VegGen Edit Mode"),
		NAME_None,
		FEditorStyle::GetStyleSetName()
	)
	{}

	//VegGenCommands

		/** Commands for the foliage brush settings. */
	TSharedPtr< FUICommandInfo > IncreaseBrushSize;
	TSharedPtr< FUICommandInfo > DecreaseBrushSize;

	/** Commands for the tools toolbar. */
	TSharedPtr< FUICommandInfo > SetPaint;
	TSharedPtr< FUICommandInfo > SetReapplySettings;
	TSharedPtr< FUICommandInfo > SetSelect;
	TSharedPtr< FUICommandInfo > SetLassoSelect;
	TSharedPtr< FUICommandInfo > SetPaintBucket;

	/** Commands for the foliage item toolbar. */
	TSharedPtr< FUICommandInfo > SetNoSettings;
	TSharedPtr< FUICommandInfo > SetPaintSettings;
	TSharedPtr< FUICommandInfo > SetClusterSettings;

	/** FoliageType commands */
	TSharedPtr< FUICommandInfo > RemoveFoliageType;
	TSharedPtr< FUICommandInfo > ShowFoliageTypeInCB;
	TSharedPtr< FUICommandInfo > SelectAllInstances;
	TSharedPtr< FUICommandInfo > DeselectAllInstances;
	TSharedPtr< FUICommandInfo > SelectInvalidInstances;
	/**
	 * Initialize commands
	 */
	virtual void RegisterCommands() override;

public:

};