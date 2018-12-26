// Fill out your copyright notice in the Description page of Project Settings.

#include "LindenmayerSystem/Source/LindenmayerSystem/Public/LSystemFoliageFactory.h"
#include "LindenmayerSystem/Source/LindenmayerSystem/Public/LSystemFoliage.h"
#include "AssetTypeCategories.h"

#define LOCTEXT_NAMESPACE "ActorFactoryLSystemFoliage"


ULSystemFoliageFactory::ULSystemFoliageFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DisplayName = LOCTEXT("LSystemActorDisplayName", "Lindenmayer System Foliage");
	NewActorClass = ALSystemFoliage::StaticClass();	
	bUseSurfaceOrientation = true;
}

bool ULSystemFoliageFactory::PreSpawnActor(UObject* Asset, FTransform& InOutLocation)
{
	NewActorClass = ALSystemFoliage::StaticClass();
	return true;
}

AActor * ULSystemFoliageFactory::SpawnActor(UObject * Asset, ULevel * InLevel, const FTransform & Transform, EObjectFlags ObjectFlags, const FName Name)
{






	return nullptr;
}



void ULSystemFoliageFactory::PostSpawnActor(UObject* Asset, AActor* NewActor)
{
	Super::PostSpawnActor(Asset, NewActor);
	//ULSystemFoliageSpawner* LSystemSpawner = CastChecked<ULSystemFoliageSpawner>(Asset);

	//UE_LOG(LogActorFactory, Log, TEXT("Actor Factory created %s"), *LSystemSpawner->GetName());

	// Change properties
	ALSystemFoliage* LSF = CastChecked<ALSystemFoliage>(NewActor);
	//ULSystemComponent* LSystemComponent = LSV->LSystemComponent;
	//check(LSystemComponent);

	//LSystemComponent->UnregisterComponent();

	//LSystemComponent->LSystemSpawner = LSystemSpawner;

	// Init Component
	//LSystemComponent->RegisterComponent();
}


void ULSystemFoliageFactory::PostCreateBlueprint(UObject* Asset, AActor* CDO)
{
	if (Asset != nullptr && CDO != nullptr)
	{
		//ULSystemFoliageSpawner* LSystemSpawner = CastChecked<ULSystemFoliageSpawner>(Asset);
		ALSystemFoliage* LSF = CastChecked<ALSystemFoliage>(CDO);
		//ULSystemComponent* ProceduralComponent = LSV->LSystemComponent;
		//ProceduralComponent->LSystemSpawner = LSystemSpawner;
	}
}


//UObject* ULSystemFoliageFactory::GetAssetFromActorInstance(AActor* ActorInstance)
//{
//	
//}



#undef LOCTEXT_NAMESPACE