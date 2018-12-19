// Fill out your copyright notice in the Description page of Project Settings.

#include "LSystemTile.h"
#include "LSystemFoliage.h"


#define LOCTEXT_NAMESPACE "Lindenmayer"

ULSystemTile::ULSystemTile(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

bool ULSystemTile::HandleOverlaps(ALSystemFoliage* Instance)
{
	//TODO
	// If the instance survives we mark all dominated overlaps as pending removal. They will be removed from the broadphase and will not spread seeds or age.
	// Note that this introduces potential indeterminism! If the iteration order changes we could get different results. This is needed because it gives us huge performance savings.
	// Note that if the underlying data structures stay the same (i.e. no core engine changes) this should not matter. This gives us short term determinism, but not long term.

	bool bSurvived = true;
	TArray<FLSystemFoliageOverlap> Overlaps;
	Broadphase.GetOverlaps(Instance, Overlaps);

	for(const FLSystemFoliageOverlap& Overlap : Overlaps)
	{
		ALSystemFoliage* Dominated = ALSystemFoliage::Domination(Overlap.A, Overlap.B, Overlap.OverlapType);
		if (Dominated == Instance)
		{
			bSurvived = false;
			break;
		}
	}

	if (bSurvived)
	{
		for (const FLSystemFoliageOverlap& Overlap : Overlaps)
		{
			if (ALSystemFoliage* Dominated = ALSystemFoliage::Domination(Overlap.A, Overlap.B, Overlap.OverlapType))
			{
				// Should only be here if we didn't survive
				check(Dominated != Instance);

				//We can't immediately remove because we're potentially iterating over existing instances.
				MarkPendingRemoval(Dominated);
			}
		}
	}
	else
	{
		MarkPendingRemoval(Instance);
	}



	return bSurvived;
}

ALSystemFoliage * ULSystemTile::NewSeed(const FVector & Location, FVector Size, ELSystemType Type, int InAge, bool bBlocker)
{
	const FVector InitRadius = 100.f * Size; //TODO get maxradius instead of 100.f
	{
		ALSystemFoliage* NewInst = new ALSystemFoliage();
		NewInst->GetActorLocation() = Location;

		// make a new local random stream to avoid changes to instance randomness changing the position of all other procedural instances
		FRandomStream LocalStream = RandomStream;
		RandomStream.GetUnsignedInt(); // advance the parent stream by one

		FRotator Rotation = {0,0,0};
		Rotation.Yaw   = LocalStream.FRandRange(0,360);
		Rotation.Pitch = LocalStream.FRandRange(0, 5);
		NewInst->GetTransform().SetRotation(FQuat(Rotation));
		NewInst->Initialize(InAge, Type);
		
		NewInst->GetTransform().SetScale3D(Size);
		NewInst->bBlocker = bBlocker;
		
		// Add the seed if possible
		Broadphase.Insert(NewInst);
		const bool bSurvived = HandleOverlaps(NewInst);
		return bSurvived ? NewInst : nullptr;
	}

	return nullptr;
}

float ULSystemTile::GetRandomGaussian()
{
	const float Rand1 = FMath::Max<float>(RandomStream.FRand(), SMALL_NUMBER);
	const float Rand2 = FMath::Max<float>(RandomStream.FRand(), SMALL_NUMBER);
	const float SqrtLn = FMath::Sqrt(-2.f * FMath::Loge(Rand1));
	const float Rand2TwoPi = Rand2 * 2.f * PI;
	const float Z1 = SqrtLn * FMath::Cos(Rand2TwoPi);
	return Z1;
}

FVector ULSystemTile::GetSeedOffset(const ALSystemFoliage* Inst, float MinDistance)
{
	//We want 10% of seeds to be the max distance so we use a z score of +- 1.64
	const float MaxZScore = 1.64f;
	const float Z1 = GetRandomGaussian();
	const float Z1Clamped = FMath::Clamp(Z1, -MaxZScore, MaxZScore);
	const float VariationDistance = Z1Clamped * Inst->GetSpreadVariance() / MaxZScore;
	const float AverageDistance = MinDistance + Inst->GetAvSpreadVariance();
	
	const float RandRad = FMath::Max<float>(RandomStream.FRand(), SMALL_NUMBER) * PI * 2.f;
	const FVector Dir = FVector(FMath::Cos(RandRad), FMath::Sin(RandRad), 0);
	return Dir * (AverageDistance + VariationDistance);
}


void ULSystemTile::AgeSeeds()
{
	TArray<ALSystemFoliage*> NewSeeds;
	for (ALSystemFoliage* Instance : InstancesSet)
	{
		if (UserCancelled()){ return; }
		if (Instance->IsAlive())
		{
			Instance->AgeFoliage(Instance->GetGen() + 1);
		}
	}


	// Get rid of the old younger versions
	FlushPendingRemovals();
}

void ULSystemTile::SpreadSeeds(TArray<ALSystemFoliage*>& NewSeeds)
{

	//TODO Make sure Foliage spreads seeds
	for (ALSystemFoliage* Inst : InstancesSet)
	{
		if (UserCancelled()){ return; }
		if (Inst->IsAlive() == false)
		{
			 //The instance has been killed so don't bother spreading seeds. 
			// Note this introduces potential indeterminism if the order of instance traversal changes (implementation details of TSet for example)
			continue;
		}

		//const UFoliageType_InstancedStaticMesh* Type = Inst->Type;

		if (SimulationStep <= Inst->NumSteps  && Inst->bSpawnsInShade == bSimulateOnlyInShade)
		{
			for (int32 i = 0; i < Inst->SeedsPerStep; ++i)
			{
				//spread new seeds
				const float NewAge = Inst->GetInitAge(RandomStream);
				const float NewScale = Type->GetScaleForAge(NewAge);
				const float MinDistanceToClear = GetSeedMinDistance(Inst, NewAge, SimulationStep);
				const FVector GlobalOffset = GetSeedOffset(Type, MinDistanceToClear);
				
				if (GlobalOffset.SizeSquared2D() + SMALL_NUMBER > MinDistanceToClear*MinDistanceToClear)
				{
					const FVector NewLocation = GlobalOffset + Inst->Location;
					if (FProceduralFoliageInstance* NewInstance = NewSeed(NewLocation, NewScale, Type, NewAge))
					{
						NewSeeds.Add(NewInstance);
					}
				}
			}
		}
	}
}



void ULSystemTile::MarkPendingRemoval(ALSystemFoliage* ToRemove)
{
		if(ToRemove->IsAlive())
		{
		Broadphase.Remove(ToRemove);
		ToRemove->TerminateInstance();
		PendingRemovals.Add(ToRemove);
		}
}

void ULSystemTile::FlushPendingRemovals()
{
}

bool ULSystemTile::UserCancelled() const
{
	return false;
}

#undef LOCTEXT_NAMESPACE

void ULSystemTile::Simulate(const ULSystemFoliageSpawner * InFoliageSpawner, const int32 RandomSeed, const int32 MaxNumSteps, const int32 InLastCancel)
{
	LastCancel = InLastCancel;
	InitSimulation(InFoliageSpawner, RandomSeed);

	RunSimulation(MaxNumSteps, false);
	RunSimulation(MaxNumSteps, true);
}

//void ULSystemTile::ExtractDesiredInstances(TArray<ALSystemFoliage>& OutDesiredInstances, const FTransform & WorldTM, const FGuid & ProceduralGuid, const float HalfHeight, const FBodyInstance * VolumeBodyInstance, bool bEmptyTileInfo)
//{
//	
//}

void ULSystemTile::CopyInstancesToTile(ULSystemTile * ToTile, const FBox2D & LocalAABB, const FTransform & RelativeTM, const float Overlap) const
{

	//@todo proc foliage: Would be better to use the max radius of any instances in the tile instead of overlap to define the outer AABB

	TArray<ALSystemFoliage*> InstancesIncludingOverlap;
	const FBox2D OuterLocalAABB(LocalAABB.Min, LocalAABB.Max + Overlap);
	
	// Get all the instances in the outer AABB (so we include potential blockers)
	GetInstancesInAABB(OuterLocalAABB, InstancesIncludingOverlap);


	ToTile->AddInstances(InstancesIncludingOverlap, RelativeTM, LocalAABB);
}

void ULSystemTile::RemoveInstance(ALSystemFoliage * Inst)
{
	if (Inst->IsAlive())
	{
		Broadphase.Remove(Inst);
		Inst->TerminateInstance();
	}
	
	InstancesSet.Remove(Inst);
	delete Inst;
}

void ULSystemTile::RemoveInstances()
{
	for (ALSystemFoliage* Inst : InstancesSet)
	{
		MarkPendingRemoval(Inst);
	}

	InstancesArray.Empty();
	FlushPendingRemovals();

}

void ULSystemTile::InitSimulation(const ULSystemFoliageSpawner * InFoliageSpawner, const int32 InRandomSeed)
{
	RandomSeed = InRandomSeed;
	RandomStream.Initialize(RandomSeed);
	FoliageSpawner = InFoliageSpawner;
	SimulationStep = 0;
	//Broadphase = FLSystemBroadPhase(FoliageSpawner->TileSize, FoliageSpawner->MinimumQuadTreeSize);
}

void ULSystemTile::GetResourceSizeEx(FResourceSizeEx & CumulativeResourceSize)
{
	Super::GetResourceSizeEx(CumulativeResourceSize);

	for (ALSystemFoliage* Inst : InstancesSet)
	{
		CumulativeResourceSize.AddDedicatedSystemMemoryBytes(sizeof(ALSystemFoliage));
	}
}

void ULSystemTile::BeginDestroy()
{
	Super::BeginDestroy();
	RemoveInstances();
}

void ULSystemTile::GetInstancesInAABB(const FBox2D & LocalAABB, TArray<ALSystemFoliage*>& OutInstances, bool bFullyContainedOnly) const
{
	TArray<ALSystemFoliage*> InstancesInAABB;
	//Broadphase.GetInstancesInBox(LocalAABB, InstancesInAABB);

	OutInstances.Reserve(OutInstances.Num() + InstancesInAABB.Num());
	for (ALSystemFoliage* Inst : InstancesInAABB)
	{


		//Check if they are fully in the AABB
		auto box = Inst->GetBounds();
		auto Location = box.GetCenter();
		auto Rad = box.GetSize();

		if (!bFullyContainedOnly || (Location.X - Rad.X >= LocalAABB.Min.X && Location.X + Rad.X <= LocalAABB.Max.X && Location.Y - Rad.Y >= LocalAABB.Min.Y && Location.Y + Rad.Y <= LocalAABB.Max.Y))
		{
			OutInstances.Add(Inst);
		}
	}

	// Sort the instances by location.
	// This protects us from any future modifications made to the broadphase that would impact the order in which instances are located in the AABB.
	OutInstances.Sort([](const ALSystemFoliage& A, const ALSystemFoliage& B)
	{
		return (B.GetActorLocation().X == A.GetActorLocation().X) ? (B.GetActorLocation().Y > A.GetActorLocation().Y) : (B.GetActorLocation().X > A.GetActorLocation().X);
	});

}

void ULSystemTile::AddInstances(const TArray<ALSystemFoliage*>& NewInstances, const FTransform & ToLocalTM, const FBox2D & InnerLocalAABB)
{
	for (const ALSystemFoliage* Inst : NewInstances)
	{
		// We need the local space because we're comparing it to the AABB
		auto box = Inst->GetBounds();
		const FVector& Location = box.GetCenter();
		const auto Size = box.GetSize();
		
		// Instances in InnerLocalAABB or on the border of the max sides of the AABB will be visible and instantiated by this tile
		// Instances outside of the InnerLocalAABB are only used for rejection purposes. This is needed for overlapping tiles
		// The actual instantiation of the object will be taken care of by a different tile
		const bool bIsOutsideInnerLocalAABB = Location.X + Size.X <= InnerLocalAABB.Min.X
											|| Location.X - Size.X > InnerLocalAABB.Max.X
											|| Location.Y + Size.Y <= InnerLocalAABB.Min.Y
											|| Location.Y - Size.Y > InnerLocalAABB.Max.Y;

		const FVector NewLocation = ToLocalTM.TransformPosition(Inst->GetActorLocation());
		if (ALSystemFoliage* NewInst = NewSeed(NewLocation, Size, Inst->GetType(), Inst->GetGen(), bIsOutsideInnerLocalAABB))
		{			
			InstancesSet.Add(NewInst);
		}
	}

	FlushPendingRemovals();
}

void ULSystemTile::InstancesToArray()
{
}

void ULSystemTile::Empty()
{
}

void ULSystemTile::RunSimulation(const int32 MaxNumSteps, bool bOnlyInShade)
{
}

void ULSystemTile::StepSimulation()
{
}

void ULSystemTile::AddRandomSeeds(TArray<ALSystemFoliage*>& OutInstances)
{
}

#undef LOCTEXT_NAMESPACE
