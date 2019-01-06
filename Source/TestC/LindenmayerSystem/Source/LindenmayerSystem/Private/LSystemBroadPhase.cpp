// Fill out your copyright notice in the Description page of Project Settings.

#include "LSystemBroadPhase.h"
#include "LindenmayerSystem/Source/LindenmayerSystem/Public/LindemayerFoliageType.h"
#include "LSystemFoliageInstance.h"


FLSystemBroadPhase::FLSystemBroadPhase(float TileSize, float MinimumQuadTreeSize)
	: QuadTree(FBox2D(FVector2D(-TileSize * 2.f, -TileSize * 2.f), FVector2D(TileSize * 2.f, TileSize * 2.f)), MinimumQuadTreeSize)
{
}

FLSystemBroadPhase::FLSystemBroadPhase(const FLSystemBroadPhase & otherBroadPhase)
: QuadTree(FBox2D(FVector2D(0, 0), FVector2D(0, 0)))
{
	otherBroadPhase.QuadTree.Duplicate(QuadTree);
}

void FLSystemBroadPhase::Empty()
{
	QuadTree.Empty();
}

FBox2D GetMaxAABB(FLSysFolInstance* Instance)
{
	const float Radius = Instance->Type->GetMaxRadius();
	const FVector2D Location(Instance->Location);
	const FVector2D Offset(Radius, Radius);
	const FBox2D AABB(Location - Offset, Location + Offset);
	return AABB;
}

void FLSystemBroadPhase::Insert(FLSysFolInstance * Instance)
{
	const FBox2D MaxAABB = GetMaxAABB(Instance);
	QuadTree.Insert(Instance, MaxAABB);
}

bool CircleOverlap(const FVector& ALocation, float ARadius, const FVector& BLocation, float BRadius)
{
	return (ALocation - BLocation).SizeSquared2D() <= (ARadius + BRadius)*(ARadius + BRadius);
}

bool FLSystemBroadPhase::GetOverlaps(FLSysFolInstance * Instance, TArray<FLSysFoliageOverlap>& Overlaps) const
{
	const float AShadeRadius     = Instance->Type->ShadeRadius;
	const float ACollisionRadius = Instance->Type->CollisionRadius;

	TArray<FLSysFolInstance*> PossibleOverlaps;
	const FBox2D AABB = GetMaxAABB(Instance);
	QuadTree.GetElements(AABB, PossibleOverlaps);
	Overlaps.Reserve(Overlaps.Num() + PossibleOverlaps.Num());


	for (FLSysFolInstance* Overlap : PossibleOverlaps)
	{
		if (Overlap != Instance)
		{
			//We must determine if this is an overlap of shade or an overlap of collision. If both the collision overlap wins
			bool bCollisionOverlap = CircleOverlap(Instance->Location, ACollisionRadius, Overlap->Location, Overlap->Type->CollisionRadius);
			bool bShadeOverlap     = CircleOverlap(Instance->Location, AShadeRadius, Overlap->Location, Overlap->Type->ShadeRadius);

			if (bCollisionOverlap || bShadeOverlap)
			{
				new (Overlaps)FLSysFoliageOverlap(Instance, Overlap, bCollisionOverlap ? ELSysSimulationOverlap::CollisionOverlap : ELSysSimulationOverlap::ShadeOverlap);
			}
			
		}
	}

	return Overlaps.Num() > 0;
}

void FLSystemBroadPhase::GetInstancesInBox(const FBox2D & Box, TArray<FLSysFolInstance*>& Instances) const
{ 
	QuadTree.GetElements(Box, Instances);
}

void FLSystemBroadPhase::Remove(FLSysFolInstance * Instance)
{
	const FBox2D AABB = GetMaxAABB(Instance);
	const bool bRemoved = QuadTree.Remove(Instance, AABB);
	check(bRemoved);
}


