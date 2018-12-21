// Fill out your copyright notice in the Description page of Project Settings.


#include "LindenmayerSystem/Source/LindenmayerSystem/Public/LSystemSpawnerFactory.h"
#include "AssetTypeCategories.h"
#include "LindenmayerSystem/Source/LindenmayerSystem/Public/LSystemFoliageSpawner.h"


ULSystemSpawnerFactory::ULSystemSpawnerFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = ULSystemFoliageSpawner::StaticClass();
	
}

UObject* ULSystemSpawnerFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	auto NewProceduralFoliage = NewObject<ULSystemFoliageSpawner>(InParent, Class, Name, Flags | RF_Transactional);

	return NewProceduralFoliage;
}

uint32 ULSystemSpawnerFactory::GetMenuCategories() const
{
	return EAssetTypeCategories::Misc;
}

bool ULSystemSpawnerFactory::ShouldShowInNewMenu() const
{
	return true;
}