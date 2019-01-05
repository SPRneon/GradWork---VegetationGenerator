// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Misc/Guid.h"
#include "Components/ActorComponent.h"
#include "Helpers.h"
#include "LSystemFoliage.h"
#include "InstancedLSystemFoliage.h"
#include "LSystemComponent.generated.h"


class AVolume;
class ULSystemFoliageSpawner;
struct FBodyInstance;

/** Describes the layout of the tiles used for LSYstem foliage simulation */
struct FLSTileLayout
{
	FLSTileLayout()
		: BottomLeftX(0), BottomLeftY(0), NumTilesX(0), NumTilesY(0), HalfHeight(0.f)
	{
	}

	// The X coordinate (in whole tiles) of the bottom-left-most active tile
	int32 BottomLeftX;

	// The Y coordinate (in whole tiles) of the bottom-left-most active tile
	int32 BottomLeftY;

	// The total number of active tiles along the x-axis
	int32 NumTilesX;

	// The total number of active tiles along the y-axis
	int32 NumTilesY;
	

	float HalfHeight;
};

UCLASS(BlueprintType)
class ULSystemComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

	/** The Lsystem spawner used to generate Lsystems within this volume. */
	UPROPERTY(Category = "Lindenmayer", BlueprintReadWrite, EditAnywhere)
	ULSystemFoliageSpawner* LSystemSpawner;

	/** The amount of overlap between simulation tiles (in cm). */
	UPROPERTY(Category = "Lindenmayer", BlueprintReadWrite, EditAnywhere)
	float TileOverlap;


#if WITH_EDITORONLY_DATA

	/** Whether to place foliage on landscape */
	UPROPERTY(Category = "Lindenmayer", BlueprintReadWrite, EditAnywhere)
	bool bAllowLandscape;

	/** Whether to place foliage on BSP */
	UPROPERTY(Category = "Lindenmayer", BlueprintReadWrite, EditAnywhere)
	bool bAllowBSP;

	/** Whether to place foliage on StaticMesh */
	UPROPERTY(Category = "Lindenmayer", BlueprintReadWrite, EditAnywhere)
	bool bAllowStaticMesh;

	/** Whether to place foliage on translucent geometry */
	UPROPERTY(Category = "Lindenmayer", BlueprintReadWrite, EditAnywhere)
	bool bAllowTranslucent;

	/** Whether to place foliage on other blocking foliage geometry */
	UPROPERTY(Category = "Lindenmayer", BlueprintReadWrite, EditAnywhere)
	bool bAllowFoliage;

	/** Whether to visualize the tiles used for the foliage spawner simulation */
	UPROPERTY(Category = "Lindenmayer", BlueprintReadWrite, EditAnywhere)
	bool bShowDebugTiles;
#endif

	// UObject interface
	virtual void PostEditImport() override;

	//TODO: Incorporate ALSYstemFoliage in ULEVEL
		/** 
	 * Runs the procedural foliage simulation to generate a list of desired instances to spawn.
	 * @return True if the simulation succeeded
	 */
	bool GenerateProceduralContent(TArray<FDesiredLSysInstance>& OutLSystemTypes);
	
	/** Removes all spawned foliage instances in the level that were spawned by this component */
	void RemoveProceduralContent();

	/** @return True if any foliage instances in the level were spawned by this component */
	bool HasSpawnedAnyInstances();

	/** @return The position in world space of the bottom-left corner of the bottom-left-most active tile */
	FVector GetWorldPosition() const;

	/** @return The bounds of area encompassed by the simulation */
	virtual FBox GetBounds() const;

	/** @return The body instanced used for bounds checking */
	FBodyInstance* GetBoundsBodyInstance() const;

	/** Determines the basic layout of the tiles used in the simulation */
	void GetTileLayout(FLSTileLayout& OutTileLayout) const;

	/**@returns the spawning volume**/
	AVolume* GetSpawningVolume() const {return SpawningVolume;}

	void SetSpawningVolume(AVolume* InSpawningVolume) { SpawningVolume = InSpawningVolume; }
	const FGuid& GetProceduralGuid() const { return ProceduralGuid; }

private:
	/** Does all the actual work of executing the procedural foliage simulation */
	bool ExecuteSimulation(TArray<FDesiredLSysInstance>& OutFoliageInstances);

private:
	UPROPERTY()
	AVolume* SpawningVolume;

	UPROPERTY()
	FGuid ProceduralGuid;
};
