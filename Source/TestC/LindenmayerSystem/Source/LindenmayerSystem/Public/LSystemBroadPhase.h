// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LSystemFoliage.h"
#include "GenericQuadTree.h"

struct FLSysFolInstance;
struct FLSysFoliageOverlap;
/**
 * 
 */
class FLSystemBroadPhase
{
public:
	FLSystemBroadPhase(float TileSize = 0.f, float MinimumQuadTreeSize = 100.f);
	FLSystemBroadPhase(const FLSystemBroadPhase& otherBroadPhase);

	void Insert(FLSysFolInstance* Instance);
	bool GetOverlaps(FLSysFolInstance* Instance, TArray<FLSysFoliageOverlap>& Overlaps) const;
	void GetInstancesInBox(const FBox2D& Box, TArray<FLSysFolInstance*>& Instances) const;
	void Remove(FLSysFolInstance* Instance);
	void Empty();


private:
	TQuadTree<FLSysFolInstance*, 4> QuadTree;

};
