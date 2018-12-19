// Fill out your copyright notice in the Description page of Project Settings.

#include "LSystemFoliageSpawner.h"
#include "Misc/FeedbackContext.h"
#include "Engine/World.h"
#include "LSystemComponentDetails.h"
#include "LSystemFoliage.h"
#include "Serialization/CustomVersion.h"
#include "Async/Async.h"

#define LOCTEXT_NAMESPACE "LSystem"

ULSystemFoliageSpawner::ULSystemFoliageSpawner(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TileSize = 10000;	//100 m
	MinimumQuadTreeSize = 100.f;
	NumUniqueTiles = 10;
	RandomSeed = 42;


	 ////Custom properties
  //   FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	 ////PropertyModule.RegisterCustomPropertyTypeLayout("LSystemFoliageSpawner", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FLSystemComponentDetails::MakeInstance));
	 //PropertyModule.RegisterCustomClassLayout("LSystemFoliageSpawner", FOnGetDetailCustomizationInstance::CreateStatic(&FLSystemComponentDetails::MakeInstance));
	 //UE_LOG(LogTemp,Log,TEXT("Went in the constructor"));
}

#if WITH_EDITOR
void ULSystemFoliageSpawner::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	bNeedsSimulation = true;
}
#endif

void ULSystemFoliageSpawner::Simulate(int32 NumSteps)
{
	/*RandomStream.Initialize(RandomSeed);
	 FVector Location(0.0f, 0.0f, 0.0f);
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = this;
	GetWorld()->SpawnActor<ALSystemFoliage>(Location,Rotation,SpawnInfo);*/

	UE_LOG(LogTemp,Log,TEXT("Simulate in spawner"));
}

void ULSystemFoliageSpawner::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	//Ar.UsingCustomVersion(FProceduralFoliageCustomVersion::GUID);
}

#undef LOCTEXT_NAMESPACE