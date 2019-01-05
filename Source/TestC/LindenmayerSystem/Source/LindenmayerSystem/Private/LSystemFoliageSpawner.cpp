// Fill out your copyright notice in the Description page of Project Settings.

#include "LSystemFoliageSpawner.h"
#include "Misc/FeedbackContext.h"
#include "Engine/World.h"
#include  "LSystemTile.h"
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



}

#if WITH_EDITOR
void ULSystemFoliageSpawner::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	bNeedsSimulation = true;
}
#endif

void ULSystemFoliageSpawner::Simulate(int32 NumSteps)
{
	RandomStream.Initialize(RandomSeed);
	CreateProceduralFoliageInstances();


	LastCancel.Increment();

	PrecomputedTiles.Empty();
	TArray<TFuture< ULSystemTile* >> Futures;

	for (int i = 0; i < NumUniqueTiles; ++i)
	{
		ULSystemTile* NewTile = NewObject<ULSystemTile>(this);
		const int32 RandomNumber = GetRandomNumber();
		const int32 LastCancelInit = LastCancel.GetValue();

		Futures.Add(Async<ULSystemTile*>(EAsyncExecution::ThreadPool, [=]()
		{
			NewTile->Simulate(this, RandomNumber, NumSteps, LastCancelInit);
			return NewTile;
		}));
	}

	const FText StatusMessage = LOCTEXT("SimulateProceduralFoliage", "Simulate ProceduralFoliage...");
	GWarn->BeginSlowTask(StatusMessage, true, true);
	
	const int32 TotalTasks = Futures.Num();

	bool bCancelled = false;

	for (int32 FutureIdx = 0; FutureIdx < Futures.Num(); ++FutureIdx)
	{
		// Sleep for 100ms if not ready. Needed so cancel is responsive.
		while (Futures[FutureIdx].WaitFor(FTimespan::FromMilliseconds(100.0)) == false)
		{
			GWarn->StatusUpdate(FutureIdx, TotalTasks, LOCTEXT("SimulateProceduralFoliage", "Simulate ProceduralFoliage..."));

			if (GWarn->ReceivedUserCancel() && bCancelled == false)
			{
				// Increment the thread-safe counter. Tiles compare against the original count and cancel if different.
				LastCancel.Increment();
				bCancelled = true;
			}
		}

		// Even if canceled, block until all threads have exited safely. This ensures memory isn't GC'd.
		PrecomputedTiles.Add(Futures[FutureIdx].Get());		
	}

	GWarn->EndSlowTask();

	if (bCancelled)
	{
		PrecomputedTiles.Empty();
	}
	else
	{
		SetClean();
	}

	UE_LOG(LogTemp,Log,TEXT("Simulate in spawner"));
}

int32 ULSystemFoliageSpawner::GetRandomNumber()
{
	return RandomStream.FRand() * RAND_MAX;
}

void ULSystemFoliageSpawner::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	//Ar.UsingCustomVersion(FProceduralFoliageCustomVersion::GUID);
}

void ULSystemFoliageSpawner::SimulateIfNeeded()
{
	Simulate();
}

const ULSystemTile * ULSystemFoliageSpawner::GetRandomTile(int32 X, int32 Y)
{
	if (PrecomputedTiles.Num())
	{
		// Random stream to use as a hash function
		FRandomStream HashStream;	
		
		HashStream.Initialize(X);
		const float XRand = HashStream.FRand();
		
		HashStream.Initialize(Y);
		const float YRand = HashStream.FRand();
		
		const int32 RandomNumber = (RAND_MAX * XRand / (YRand + 0.01f));
		const int32 Idx = FMath::Clamp(RandomNumber % PrecomputedTiles.Num(), 0, PrecomputedTiles.Num() - 1);
		return PrecomputedTiles[Idx].Get();
	}

	return nullptr;
}

ULSystemTile * ULSystemFoliageSpawner::CreateTempTile()
{
	ULSystemTile* TmpTile = NewObject<ULSystemTile>(this);
	TmpTile->InitSimulation(this, 0);

	return TmpTile;
}

void ULSystemFoliageSpawner::CreateProceduralFoliageInstances()
{
	for (auto FoliageObject : LSystemInstances)
	{
		// Refresh the instances contained in the type objects
		FoliageObject->ResimulateTree();
	}

}

void ULSystemFoliageSpawner::SetClean()
{
	/*
	for (ALSystemFoliage* FoliageObject : LSystemInstances)
	{
		FoliageObject.SetClean();
	}*/

	bNeedsSimulation = false;
}


#undef LOCTEXT_NAMESPACE