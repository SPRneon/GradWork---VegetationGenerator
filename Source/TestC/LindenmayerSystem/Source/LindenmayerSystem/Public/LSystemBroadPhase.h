// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LSystemFoliage.h"
#include "GenericQuadTree.h"


/**
 * 
 */
class FLSystemBroadPhase
{
public:
	FLSystemBroadPhase(float TileSize = 0.f, float MinimumQuadTreeSize = 100.f);
	FLSystemBroadPhase(const FLSystemBroadPhase& otherBroadPhase);

	void Insert(ALSystemFoliage* Instance);
	bool GetOverlaps(ALSystemFoliage* Instance, TArray<FLSystemFoliageOverlap>& Overlaps) const;
	void GetInstancesInBox(const FBox2D& Box, TArray<ALSystemFoliage*>& Instances) const;
	void Remove(ALSystemFoliage* Instance);
	void Empty();


private:
	TQuadTree<ALSystemFoliage*, 4> QuadTree;

};
