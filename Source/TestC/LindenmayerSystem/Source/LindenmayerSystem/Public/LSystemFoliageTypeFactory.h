// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Factories/Factory.h"
#include "LSystemFoliageTypeFactory.generated.h"

/**
 * 
 */
UCLASS()
class  ULSystemFoliageTypeFactory : public UFactory
{
	GENERATED_UCLASS_BODY()
	
	// UFactory interface
	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual uint32 GetMenuCategories() const override;
	virtual bool ShouldShowInNewMenu() const override;
	// End of UFactory interface
	
};
