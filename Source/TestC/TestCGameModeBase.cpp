// Fill out your copyright notice in the Description page of Project Settings.

#include "TestCGameModeBase.h"
#include "PropertyEditorModule.h"
#include "ModuleManager.h"
#include "LSystemComponentDetails.h"


void ATestCGameModeBase::OnConstruction(const FTransform& Transform)
{
	AGameModeBase::OnConstruction(Transform);

	 //Custom properties
     FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	 //PropertyModule.RegisterCustomPropertyTypeLayout("LSystemFoliageSpawner", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FLSystemComponentDetails::MakeInstance));
	 PropertyModule.RegisterCustomClassLayout("LSystemFoliageSpawner", FOnGetDetailCustomizationInstance::CreateStatic(&FLSystemComponentDetails::MakeInstance));
	 UE_LOG(LogTemp,Log,TEXT("Went in the constructor"));


}

