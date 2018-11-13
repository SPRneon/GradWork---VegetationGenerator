// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"




class IVegetationGeneratorPluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

//#if WITH_EDITOR
//	/** Move the selected foliage to the specified level */
//	virtual void MoveSelectedFoliageToLevel(ULevel* InTargetLevel) = 0;
//	virtual bool CanMoveSelectedFoliageToLevel(ULevel* InTargetLevel) const = 0;
//#endif
};