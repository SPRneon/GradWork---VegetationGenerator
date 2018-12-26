// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Editor/PropertyEditor/Public/IDetailCustomization.h"
#include "Editor/PropertyEditor/Public/DetailLayoutBuilder.h"
#include "HAL/ThreadSafeCounter.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "Math/RandomStream.h"
#include "Helpers.h"
#include "LSystemFoliage.h"
#include "LindemayerFoliageType.h"
#include "LSystemFoliageSpawner.generated.h"


class UProceduralFoliageTile;


/////////////////////////////////////////
UCLASS(BlueprintType, Blueprintable)
class ULSystemFoliageSpawner : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/** The seed used for generating the randomness of the simulation. */
	UPROPERTY(Category = Lindenmayer, EditAnywhere, BlueprintReadOnly)
	int32 RandomSeed;

	/** Length of the tile (in cm) along one axis. The total area of the tile will be TileSize*TileSize. */
	UPROPERTY(Category = Lindenmayer, EditAnywhere, BlueprintReadOnly)
	float TileSize;

	/** The number of unique tiles to generate. The final simulation is a procedurally determined combination of the various unique tiles. */
	UPROPERTY(Category = Lindenmayer, EditAnywhere, BlueprintReadOnly)
	int32 NumUniqueTiles;

	/** Minimum size of the quad tree used during the simulation. Reduce if too many instances are in splittable leaf quads (as warned in the log). */
	UPROPERTY(Category = Lindenmayer, EditAnywhere, BlueprintReadOnly)
	float MinimumQuadTreeSize;

	FThreadSafeCounter LastCancel;

private:
	UPROPERTY(Category = Lindenmayer, EditAnywhere)
	TArray<ULSystemFoliageType*> m_LSystemTypes;

	UPROPERTY()
	bool bNeedsSimulation;

public:
	//Functions
	UFUNCTION(BlueprintCallable, Category = Lindenmayer)
	void Simulate(int32 NumSteps = -1);

	int32 GetRandomNumber();

	const TArray<ULSystemFoliageType*> GetFoliageTypes() const { return m_LSystemTypes; }

	//void GetInstancesToSpawn(TArray<FProceduralFoliageInstance>& OutInstances, const FVector& Min = FVector(-FLT_MAX, -FLT_MAX, -FLT_MAX), const FVector& Max = FVector(FLT_MAX, FLT_MAX, FLT_MAX) ) const;

	#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);
#endif

	virtual void Serialize(FArchive& Ar);

	/** Simulates tiles if needed */
	void SimulateIfNeeded();

	/** Takes a tile index and returns a random tile associated with that index. */
	const ULSystemTile* GetRandomTile(int32 X, int32 Y);

	/** Creates a temporary empty tile with the appropriate settings created for it. */
	ULSystemTile* CreateTempTile();
private:
	private:
	void CreateProceduralFoliageInstances();

	bool AnyDirty() const;

	void SetClean();

private:
	TArray<TWeakObjectPtr<UProceduralFoliageTile>> PrecomputedTiles;

	FRandomStream RandomStream;


	
};
