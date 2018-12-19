// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "LSystemFoliage.h"
#include "Math/RandomStream.h"
#include "LSystemBroadPhase.h"
#include "LSystemTile.generated.h"


class ULSystemFoliageSpawner;
struct FBodyInstance;
/**
 * 
 */
UCLASS()
class TESTC_API ULSystemTile : public UObject
{
	GENERATED_UCLASS_BODY()

	//Simulates the placement of the foliage
	void Simulate(const ULSystemFoliageSpawner* InFoliageSpawner, const int32 RandomSeed, const int32 MaxNumSteps, const int32 InLastCancel);

	//Gets info of all isntances
	//void ExtractDesiredInstances(TArray<ALSystemFoliage>& OutDesiredInstances, const FTransform& WorldTM, const FGuid& ProceduralGuid, const float HalfHeight, const FBodyInstance* VolumeBodyInstance, bool bEmptyTileInfo = true);

	//Copies instances of this tile to another
	void CopyInstancesToTile(ULSystemTile* ToTile, const FBox2D& LocalAABB, const FTransform& RelativeTM, const float Overlap) const;


	/**
	 *	Removes a single instance from the tile
	 *	@param Inst The instance to remove
	 */
	void RemoveInstance(ALSystemFoliage* Inst);

	/** Removes all instances from the tile */
	void RemoveInstances();

	/** Initializes the tile for simulation. Called automatically by Simulate, so only bother to call if the tile won't actually simulate. */
	void InitSimulation(const ULSystemFoliageSpawner* InFoliageSpawner, const int32 InRandomSeed);

	// UObject interface
	virtual void GetResourceSizeEx(FResourceSizeEx& CumulativeResourceSize) override;
	virtual void BeginDestroy() override;

private:
	/**
	* Finds all the instances within an AABB. The instances are returned in the space local to the AABB.
	* That is, an instance in the bottom left AABB will have a coordinate of (0,0).
	*
	* @param LocalAABB The AABB to get instances within
	* @param OutInstances Contains the instances within the specified AABB
	* @param bFullyContainedOnly If true, will only get instances that are fully contained within the AABB
	*/
	void GetInstancesInAABB(const FBox2D& LocalAABB, TArray<ALSystemFoliage* >& OutInstances, bool bFullyContainedOnly = true) const;

	/**
	 * Adds procedural instances to the tile from some external source.
	 * @param The new instances to add to the tile
	 * @param ToLocalTM Used to transform the new instances to this tile's local space
	 * @param InnerLocalAABB New instances outside this region will be tracked for blocking purposes only (and never spawned)
	 */
	void AddInstances(const TArray<ALSystemFoliage*>& NewInstances, const FTransform& ToLocalTM, const FBox2D& InnerLocalAABB);

	/** Fills the InstancesArray by iterating through the contents of the InstancesSet */
	void InstancesToArray();

	/** Empty arrays and sets */
	void Empty();

	/** Run the simulation for the appropriate number of steps. 
	 * @param bOnlyInShade Whether the simulation should run exclusively within existing shade from other instances
	 */
	void RunSimulation(const int32 MaxNumSteps, bool bOnlyInShade);

	/** Advance the simulation by a step (think of a step as a generation) */
	void StepSimulation();

	/** Randomly seed the next step of the simulation */
	void AddRandomSeeds(TArray<ALSystemFoliage*>& OutInstances);

	/** Determines whether the instance will survive all the overlaps, and then kills the appropriate instances. Returns true if the instance survives */
	bool HandleOverlaps(ALSystemFoliage* Instance);

	/** Attempts to create a new instance and resolves any overlaps. Returns the new instance if successful for calling code to add to Instances */
	ALSystemFoliage* NewSeed(const FVector& Location, FVector Size, ELSystemType Type, int InAge, bool bBlocker = false);

	void SpreadSeeds(TArray<ALSystemFoliage*>& NewSeeds);
	void AgeSeeds();

	void MarkPendingRemoval(ALSystemFoliage* ToRemove);
	void FlushPendingRemovals();

	bool UserCancelled() const;

private:

	UPROPERTY()
	const ULSystemFoliageSpawner* FoliageSpawner;

	TSet<ALSystemFoliage*> PendingRemovals;
	TSet<ALSystemFoliage*> InstancesSet;

	UPROPERTY()
	TArray<ALSystemFoliage*> InstancesArray;

	int32 SimulationStep;
	FLSystemBroadPhase Broadphase;	

	int32 RandomSeed;
	FRandomStream RandomStream;
	bool bSimulateOnlyInShade;

	int32 LastCancel;

private:
	float GetRandomGaussian();
	FVector GetSeedOffset(const ALSystemFoliage* Inst, float MinDistance);
};
