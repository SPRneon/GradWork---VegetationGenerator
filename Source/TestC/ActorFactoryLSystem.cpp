// Fill out your copyright notice in the Description page of Project Settings.


/*=============================================================================
ActorFactory.cpp:
=============================================================================*/

#include "ActorFactoryLSystem.h"
#include "LSystemFoliageSpawner.h"
#include "LSystemVolume.h"
#include "LSystemComponent.h"
#include "AssetData.h"
#include "Settings/EditorExperimentalSettings.h"


#define LOCTEXT_NAMESPACE "ActorFactoryLSystem"

UActorFactoryLSystem::UActorFactoryLSystem(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	DisplayName = LOCTEXT("LSystemDisplayName", "Lindenmayer System Volume");
	NewActorClass = ALSystemVolume::StaticClass();
	
	bUseSurfaceOrientation = true;
}

bool UActorFactoryLSystem::PreSpawnActor(UObject* Asset, FTransform& InOutLocation)
{
	NewActorClass = ALSystemVolume::StaticClass();
	return true;
}

bool UActorFactoryLSystem::CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg)
{
	if (!AssetData.IsValid() || !AssetData.GetClass()->IsChildOf(ULSystemFoliageSpawner::StaticClass()))
	{
		OutErrorMsg = NSLOCTEXT("CanCreateActor", "NoLSystemFoliageSpawner", "A valid LSystemFoliageSpawner must be specified.");
		return false;
	}

	return true;
}


void UActorFactoryLSystem::PostSpawnActor(UObject* Asset, AActor* NewActor)
{
	Super::PostSpawnActor(Asset, NewActor);
	ULSystemFoliageSpawner* LSystemSpawner = CastChecked<ULSystemFoliageSpawner>(Asset);

	UE_LOG(LogActorFactory, Log, TEXT("Actor Factory created %s"), *LSystemSpawner->GetName());

	// Change properties
	ALSystemVolume* LSV = CastChecked<ALSystemVolume>(NewActor);
	ULSystemComponent* LSystemComponent = LSV->LSystemComponent;
	check(LSystemComponent);

	LSystemComponent->UnregisterComponent();

	LSystemComponent->LSystemSpawner = LSystemSpawner;

	// Init Component
	LSystemComponent->RegisterComponent();
}

UObject* UActorFactoryLSystem::GetAssetFromActorInstance(AActor* Instance)
{
	check(Instance->IsA(NewActorClass));

	ALSystemVolume* LSV = CastChecked<ALSystemVolume>(Instance);
	ULSystemComponent* ProceduralComponent = LSV->LSystemComponent;
	check(ProceduralComponent);
	
	return ProceduralComponent->LSystemSpawner;
}

void UActorFactoryLSystem::PostCreateBlueprint(UObject* Asset, AActor* CDO)
{
	if (Asset != nullptr && CDO != nullptr)
	{
		ULSystemFoliageSpawner* LSystemSpawner = CastChecked<ULSystemFoliageSpawner>(Asset);
		ALSystemVolume* LSV = CastChecked<ALSystemVolume>(CDO);
		ULSystemComponent* ProceduralComponent = LSV->LSystemComponent;
		ProceduralComponent->LSystemSpawner = LSystemSpawner;
	}
}

#undef LOCTEXT_NAMESPACE

