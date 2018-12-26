// Fill out your copyright notice in the Description page of Project Settings.

#include "LSystemBroadPhase.h"
#include "LindenmayerSystem/Source/LindenmayerSystem/Public/LindemayerFoliageType.h"



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

FBox2D GetMaxAABB(ALSystemFoliage* Instance)
{
	const float Radius = Instance->Type->GetMaxRadius();
	const FVector2D Location(Instance->GetActorLocation());
	const FVector2D Offset(Radius, Radius);
	const FBox2D AABB(Location - Offset, Location + Offset);
	return AABB;
}

void FLSystemBroadPhase::Insert(ALSystemFoliage * Instance)
{
	const FBox2D MaxAABB = GetMaxAABB(Instance);
	QuadTree.Insert(Instance, MaxAABB);
}

bool CircleOverlap(const FVector& ALocation, float ARadius, const FVector& BLocation, float BRadius)
{
	return (ALocation - BLocation).SizeSquared2D() <= (ARadius + BRadius)*(ARadius + BRadius);
}

bool FLSystemBroadPhase::GetOverlaps(ALSystemFoliage * Instance, TArray<FLSystemFoliageOverlap>& Overlaps) const
{
	const float AShadeRadius     = Instance->Type->ShadeRadius;
	const float ACollisionRadius = Instance->Type->CollisionRadius;

	TArray<ALSystemFoliage*> PossibleOverlaps;
	const FBox2D AABB = GetMaxAABB(Instance);
	QuadTree.GetElements(AABB, PossibleOverlaps);
	Overlaps.Reserve(Overlaps.Num() + PossibleOverlaps.Num());


	for (ALSystemFoliage* Overlap : PossibleOverlaps)
	{
		if (Overlap != Instance)
		{
			//We must determine if this is an overlap of shade or an overlap of collision. If both the collision overlap wins
			bool bCollisionOverlap = CircleOverlap(Instance->GetActorLocation(), ACollisionRadius, Overlap->GetActorLocation(), Overlap->Type->CollisionRadius);
			bool bShadeOverlap     = CircleOverlap(Instance->GetActorLocation(), AShadeRadius, Overlap->GetActorLocation(), Overlap->Type->ShadeRadius);

			if (bCollisionOverlap || bShadeOverlap)
			{
				new (Overlaps)FLSystemFoliageOverlap(Instance, Overlap, bCollisionOverlap ? ESimulationOverlap::CollisionOverlap : ESimulationOverlap::ShadeOverlap);
			}
			
		}
	}

	return Overlaps.Num() > 0;
}

void FLSystemBroadPhase::GetInstancesInBox(const FBox2D & Box, TArray<ALSystemFoliage*>& Instances) const
{ 
	QuadTree.GetElements(Box, Instances);
}

void FLSystemBroadPhase::Remove(ALSystemFoliage * Instance)
{
	const FBox2D AABB = GetMaxAABB(Instance);
	const bool bRemoved = QuadTree.Remove(Instance, AABB);
	check(bRemoved);
}


