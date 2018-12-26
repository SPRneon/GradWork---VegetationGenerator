// Fill out your copyright notice in the Description page of Project Settings.

#include "LSystemFoliageTypeFactory.h"
#include "AssetTypeCategories.h"
#include "LindenmayerSystem/Source/LindenmayerSystem/Public/LindemayerFoliageType.h"


ULSystemFoliageTypeFactory::ULSystemFoliageTypeFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = ULSystemFoliageType::StaticClass();
	
}

UObject* ULSystemFoliageTypeFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	auto NewLSystemType = NewObject<ULSystemFoliageType>(InParent, Class, Name, Flags | RF_Transactional);

	return NewLSystemType;
}

uint32 ULSystemFoliageTypeFactory::GetMenuCategories() const
{
	return EAssetTypeCategories::Misc;
}

bool ULSystemFoliageTypeFactory::ShouldShowInNewMenu() const
{
	return true;
}