// Fill out your copyright notice in the Description page of Project Settings.

#include "LSystemTile.h"
#include "LSystemFoliage.h"
#include "LindemayerFoliageType.h"
#include "Engine/World.h"
#include "LSystemFoliageSpawner.h"
#include "LindemayerFoliageType.h"
#include "InstancedLSystemFoliage.h"





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

ALSystemFoliage * ULSystemTile::NewSeed(const FVector & Location, FVector Size,const ULSystemFoliageType* Type, int InAge, bool bBlocker)
{
	const FVector InitRadius = 100.f * Size; //TODO get maxradius instead of 100.f
	{
		
		

		// make a new local random stream to avoid changes to instance randomness changing the position of all other procedural instances
		FRandomStream LocalStream = RandomStream;
		RandomStream.GetUnsignedInt(); // advance the parent stream by one

		FRotator Rotation = {0,0,0};
		Rotation.Yaw   = LocalStream.FRandRange(0,360);
		Rotation.Pitch = LocalStream.FRandRange(0, 5);


		FActorSpawnParameters SpawnParams;
		
		//ALSystemFoliage* NewInst = GetWorld()->SpawnActor<ALSystemFoliage>(Location, Rotation,SpawnParams);

		ALSystemFoliage* NewInst = NewObject<ALSystemFoliage>();
		NewInst->m_Generation = InAge;
		NewInst->Type = Type;
		NewInst->GetTransform().SetScale3D(Size);
		NewInst->bBlocker = bBlocker;
		//NewInst->Initialize()????


		// Add the seed if possible
		Broadphase.Insert(NewInst);
		const bool bSurvived = HandleOverlaps(NewInst);
		return (bSurvived ? NewInst : nullptr);
	}

	return nullptr;
}

float ULSystemTile::GetSeedMinDistance(const ALSystemFoliage* Instance, const float NewInstanceAge, const int32 SimulationStep)
{
	const ULSystemFoliageType* Type = Instance->Type;
	const int32 StepsLeft = Type->MaxAge - SimulationStep;
	const float InstanceMaxAge = Type->GetNextAge(Instance->GetGen(), StepsLeft);
	const float NewInstanceMaxAge = Type->GetNextAge(NewInstanceAge, StepsLeft);

	const float InstanceMaxScale = Type->GetScaleForAge(InstanceMaxAge);
	const float NewInstanceMaxScale = Type->GetScaleForAge(NewInstanceMaxAge);

	const float InstanceMaxRadius = InstanceMaxScale * Type->GetMaxRadius();
	const float NewInstanceMaxRadius = NewInstanceMaxScale * Type->GetMaxRadius();

	return InstanceMaxRadius + NewInstanceMaxRadius;
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

FVector ULSystemTile::GetSeedOffset(const ULSystemFoliageType* Type, float MinDistance)
{
	//We want 10% of seeds to be the max distance so we use a z score of +- 1.64
	const float MaxZScore = 1.64f;
	const float Z1 = GetRandomGaussian();
	const float Z1Clamped = FMath::Clamp(Z1, -MaxZScore, MaxZScore);
	const float VariationDistance = Z1Clamped * Type->SpreadVariance / MaxZScore;
	const float AverageDistance = MinDistance + Type->AverageSpreadDistance;
	
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
			const ULSystemFoliageType* Type = Instance->Type;
			if (SimulationStep <= Type->NumSteps && Type->GetSpawnsInShade() == bSimulateOnlyInShade)
			{
				const float CurrentAge = Instance->GetGen();
				const float NewAge = Type->GetNextAge(Instance->GetGen(), 1);
				const float NewScale = Type->GetScaleForAge(NewAge);

				const FVector Location = Instance->GetActorLocation();

				// Replace the current instance with the newly aged version
				MarkPendingRemoval(Instance);
				if (ALSystemFoliage* Inst = NewSeed(Location, FVector(NewScale), Type, NewAge))
				{
					NewSeeds.Add(Inst);
				}
			}
		}
	}

	for (ALSystemFoliage* Seed : NewSeeds)
	{
		InstancesSet.Add(Seed);
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

		

		if (SimulationStep <= Inst->Type->NumSteps  && Inst->Type->bSpawnsInShade == bSimulateOnlyInShade)
		{
			for (int32 i = 0; i < Inst->Type->SeedsPerStep; ++i)
			{
				//spread new seeds
				const float NewAge = Inst->Type->GetInitAge(RandomStream);
				const float NewScale = Inst->Type->GetScaleForAge(NewAge);
				const float MinDistanceToClear = GetSeedMinDistance(Inst, NewAge, SimulationStep);
				const FVector GlobalOffset = GetSeedOffset(Inst->Type, MinDistanceToClear);
				
				if (GlobalOffset.SizeSquared2D() + SMALL_NUMBER > MinDistanceToClear*MinDistanceToClear)
				{
					const FVector NewLocation = GlobalOffset + Inst->GetActorLocation();
					if (ALSystemFoliage* NewInstance = NewSeed(NewLocation, FVector(NewScale), Inst->Type, NewAge))
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

	for (ALSystemFoliage* ToRemove : PendingRemovals)
	{
		RemoveInstance(ToRemove);
	}

	PendingRemovals.Empty();
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

void ULSystemTile::ExtractDesiredInstances(TArray<FDesiredLSysInstance>& OutDesiredInstances, const FTransform & WorldTM, const FGuid & ProceduralGuid, const float HalfHeight, const FBodyInstance * VolumeBodyInstance, bool bEmptyTileInfo)
{

	const FCollisionQueryParams Params(NAME_None, FCollisionQueryParams::GetUnknownStatId(), true);
	FHitResult Hit;

	OutDesiredInstances.Reserve(InstancesSet.Num());
	for (auto Instance : InstancesArray)
	{
		auto type = Instance->Type;
		FVector StartRay = Instance->GetActorLocation() + WorldTM.GetLocation();
		StartRay.Z += HalfHeight;
		FVector EndRay = StartRay;
		EndRay.Z -= (HalfHeight*2.f + 10.f);	//add 10cm to bottom position of raycast. This is needed because volume is usually placed directly on geometry and then you get precision issues

		FDesiredLSysInstance* DesiredInst = new (OutDesiredInstances)FDesiredLSysInstance(StartRay, EndRay, type->GetMaxRadius());
		DesiredInst->Rotation = FQuat( Instance->GetActorRotation());
		DesiredInst->ProceduralGuid = ProceduralGuid;
		DesiredInst->FoliageType = type;
		DesiredInst->Age = Instance->GetGen();
		DesiredInst->ProceduralVolumeBodyInstance = VolumeBodyInstance;
		DesiredInst->PlacementMode = ELSysPlacementMode::Procedural;
	}

	if (bEmptyTileInfo)
	{
		Empty();
	}
}


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
	MarkPendingRemoval(Inst);
	//delete Inst;
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
	Broadphase = FLSystemBroadPhase(FoliageSpawner->TileSize, FoliageSpawner->MinimumQuadTreeSize);
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
		if (ALSystemFoliage* NewInst = NewSeed(NewLocation, Size, Inst->Type, Inst->GetGen(), bIsOutsideInnerLocalAABB))
		{			
			InstancesSet.Add(NewInst);
		}
	}

	FlushPendingRemovals();
}

void ULSystemTile::InstancesToArray()
{
	InstancesArray.Empty(InstancesSet.Num());
	for (ALSystemFoliage* FromInst : InstancesSet)
	{
		// Blockers do not get instantiated so don't bother putting it into array
		if (FromInst->bBlocker == false)
		{
			
			auto LSA = ALSystemFoliage::StaticClass()->GetDefaultObject<ALSystemFoliage>();
			LSA = FromInst;
			//new(InstancesArray)ALSystemFoliage(*FromInst);
			InstancesArray.Push(LSA);
			
		}
	}
}

void ULSystemTile::Empty()
{
	Broadphase.Empty();
	InstancesArray.Empty();
	
	for (ALSystemFoliage* Inst : InstancesSet)
	{
		delete Inst;
	}

	InstancesSet.Empty();
	PendingRemovals.Empty();
}

void ULSystemTile::RunSimulation(const int32 MaxNumSteps, bool bOnlyInShade)
{
	int32 MaxSteps = 0;

	for (const ULSystemFoliageType* FoliageTypeObject : FoliageSpawner->GetFoliageTypes())
	{
		
		if(FoliageTypeObject->GetSpawnsInShade() == bOnlyInShade)
		{
			MaxSteps = FMath::Max(MaxSteps, FoliageTypeObject->NumSteps);

		}
	}

	if (MaxNumSteps >= 0)
	{
		MaxSteps = FMath::Min(MaxSteps, MaxNumSteps);	//only take as many steps as given
	}

	SimulationStep = 0;
	bSimulateOnlyInShade = bOnlyInShade;
	for (int32 Step = 0; Step < MaxSteps; ++Step)
	{
		StepSimulation();
		++SimulationStep;
	}

	InstancesToArray();
}

void ULSystemTile::StepSimulation()
{
	if (UserCancelled()){ return; }
	TArray<ALSystemFoliage*> NewInstances;
	if (SimulationStep == 0)
	{
		AddRandomSeeds(NewInstances);
	}
	else
	{
		AgeSeeds();
		SpreadSeeds(NewInstances);
	}

	for (ALSystemFoliage* Inst : NewInstances)
	{
		InstancesSet.Add(Inst);
	}

	FlushPendingRemovals();

}

void ULSystemTile::AddRandomSeeds(TArray<ALSystemFoliage*>& OutInstances)
{
	const float SizeTenM2 = ( FoliageSpawner->TileSize * FoliageSpawner->TileSize ) / ( 1000.f * 1000.f );

	TMap<int32,float> MaxShadeRadii;
	TMap<int32, float> MaxCollisionRadii;
	TMap<const ULSystemFoliageType*, int32> SeedsLeftMap;
	TMap<const ULSystemFoliageType*, FRandomStream> RandomStreamPerType;

	TArray<const ULSystemFoliageType*> TypesToSeed;

	for (const ULSystemFoliageType* TypeInstance : FoliageSpawner->GetFoliageTypes())
	{
		if (UserCancelled()){ return; }
		//const UFoliageType_InstancedStaticMesh* TypeInstance = FoliageTypeObject.GetInstance();
		if (TypeInstance && TypeInstance->GetSpawnsInShade() == bSimulateOnlyInShade)
		{
			{	//compute the number of initial seeds
				const int32 NumSeeds = FMath::RoundToInt(TypeInstance->GetSeedDensitySquared() * SizeTenM2);
				SeedsLeftMap.Add(TypeInstance, NumSeeds);
				if (NumSeeds > 0)
				{
					TypesToSeed.Add(TypeInstance);
				}
			}

			{	//save the random stream per type
				RandomStreamPerType.Add(TypeInstance, FRandomStream(TypeInstance->DistributionSeed + FoliageSpawner->RandomSeed + RandomSeed));
			}

			{	//compute the needed offsets for initial seed variance
				const int32 DistributionSeed = TypeInstance->DistributionSeed;
				const float MaxScale = TypeInstance->GetScaleForAge(TypeInstance->MaxAge);
				const float TypeMaxCollisionRadius = MaxScale * TypeInstance->CollisionRadius;
				if (float* MaxRadius = MaxCollisionRadii.Find(DistributionSeed))
				{
					*MaxRadius = FMath::Max(*MaxRadius, TypeMaxCollisionRadius);
				}
				else
				{
					MaxCollisionRadii.Add(DistributionSeed, TypeMaxCollisionRadius);
				}

				const float TypeMaxShadeRadius = MaxScale * TypeInstance->ShadeRadius;
				if (float* MaxRadius = MaxShadeRadii.Find(DistributionSeed))
				{
					*MaxRadius = FMath::Max(*MaxRadius, TypeMaxShadeRadius);
				}
				else
				{
					MaxShadeRadii.Add(DistributionSeed, TypeMaxShadeRadius);
				}
			}
			
		}
	}

	int32 TypeIdx = -1;
	const int32 NumTypes = TypesToSeed.Num();
	int32 TypesLeftToSeed = NumTypes;
	const int32 LastShadeCastingIndex = InstancesArray.Num() - 1; //when placing shade growth types we want to spawn in shade if possible
	while (TypesLeftToSeed > 0)
	{
		if (UserCancelled()){ return; }
		TypeIdx = (TypeIdx + 1) % NumTypes;	//keep cycling through the types that we spawn initial seeds for to make sure everyone gets fair chance

		if (const ULSystemFoliageType* Type = TypesToSeed[TypeIdx])
		{
			int32& SeedsLeft = SeedsLeftMap.FindChecked(Type);
			if (SeedsLeft == 0)
			{
				continue;
			}

			const float NewAge = Type->GetInitAge(RandomStream);
			const float Scale = Type->GetScaleForAge(NewAge);

			FRandomStream& TypeRandomStream = RandomStreamPerType.FindChecked(Type);
			float InitX = 0.f;
			float InitY = 0.f;
			float NeededRadius = 0.f;

			if (bSimulateOnlyInShade && LastShadeCastingIndex >= 0)
			{
				const int32 InstanceSpawnerIdx = TypeRandomStream.FRandRange(0, LastShadeCastingIndex);
				const ALSystemFoliage* Spawner = InstancesArray[InstanceSpawnerIdx];
				InitX = Spawner->GetActorLocation().X;
				InitY = Spawner->GetActorLocation().Y;
				NeededRadius = Spawner->Type->CollisionRadius * (Scale + Spawner->Type->GetScaleForAge(Spawner->m_Generation));
			}
			else
			{
				InitX = TypeRandomStream.FRandRange(0, FoliageSpawner->TileSize);
				InitY = TypeRandomStream.FRandRange(0, FoliageSpawner->TileSize);
				NeededRadius = MaxShadeRadii.FindRef(Type->DistributionSeed);
			}

			const float Rad = RandomStream.FRandRange(0, PI*2.f);
			
			
			const FVector GlobalOffset = (RandomStream.FRandRange(0, Type->MaxInitialSeedOffset) + NeededRadius) * FVector(FMath::Cos(Rad), FMath::Sin(Rad), 0.f);

			const float X = InitX + GlobalOffset.X;
			const float Y = InitY + GlobalOffset.Y;

			if (ALSystemFoliage* NewInst = NewSeed(FVector(X, Y, 0.f), FVector(Scale), Type, NewAge))
			{
				OutInstances.Add(NewInst);
			}

			--SeedsLeft;
			if (SeedsLeft == 0)
			{
				--TypesLeftToSeed;
			}
		}
	}

}

#undef LOCTEXT_NAMESPACE
