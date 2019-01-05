// Fill out your copyright notice in the Description page of Project Settings.

#include "LSystemVolume.h"
#include "Components/BrushComponent.h"

#include "LSystemComponent.h"
#include "LSystemFoliageSpawner.h"


static FName LSystemVolume_NAME(TEXT("LSystemVolume"));

ALSystemVolume::ALSystemVolume(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	LSystemComponent = ObjectInitializer.CreateDefaultSubobject<ULSystemComponent>(this, TEXT("LSystemComponent"));
	LSystemComponent->SetSpawningVolume(this);

	if (UBrushComponent* MyBrushComponent = GetBrushComponent())
	{
		MyBrushComponent->SetCollisionObjectType(ECC_WorldStatic);
		MyBrushComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

		// This is important because the volume overlaps with all procedural foliage
		// That means during streaming we'll get a huge hitch for UpdateOverlaps
		MyBrushComponent->bGenerateOverlapEvents = false;
	}
}

#if WITH_EDITOR

void ALSystemVolume::SpawnLSystemInstances(const TArray<FDesiredLSysInstance>& desiredInsts)
{	
	TMap<const ULSystemFoliageType*, TArray<FDesiredLSysInstance>> SettingsInstancesMap;
	for (const FDesiredLSysInstance& DesiredInst : desiredInsts)
	{
		TArray<FDesiredLSysInstance>& Instances = SettingsInstancesMap.FindOrAdd(DesiredInst.FoliageType);
		Instances.Add(DesiredInst);
	}

	for (auto It = SettingsInstancesMap.CreateConstIterator(); It; ++It)
	{
		const ULSystemFoliageType* FoliageType = It.Key();

		const TArray<FDesiredLSysInstance>& Instances = It.Value();
		AddInstances(FoliageType, Instances, TArray<int32>(), 1.f, nullptr);
	}

}

void ALSystemVolume::AddInstances(const ULSystemFoliageType* Settings, const TArray<FDesiredLSysInstance>& DesiredInstances, const TArray<int32>& ExistingInstanceBuckets, const float Pressure, LandscapeLayerCacheData* LandscapeLayerCachesPtr)
{
	if (DesiredInstances.Num() == 0)
	{
		return;
	}

	TArray<FLSysPotentialInstance> PotentialInstanceBuckets[NUM_INSTANCE_BUCKETS];
	if (DesiredInstances[0].PlacementMode == ELSysPlacementMode::Manual)
	{
		CalculatePotentialInstances(Settings, DesiredInstances, PotentialInstanceBuckets);
	}
}

void ALSystemVolume::CalculatePotentialInstances(const ULSystemFoliageType* Settings, const TArray<FDesiredLSysInstance>& DesiredInstances, TArray<FLSysPotentialInstance> OutPotentialInstances[NUM_INSTANCE_BUCKETS],  LandscapeLayerCacheData* LandscapeLayerCachesPtr)
{
	//SCOPE_CYCLE_COUNTER(STAT_FoliageCalculatePotentialInstance);
	
	LandscapeLayerCacheData LocalCache;
	LandscapeLayerCachesPtr = LandscapeLayerCachesPtr ? LandscapeLayerCachesPtr : &LocalCache;

	// Quick lookup of potential instance locations, used for overlapping check.
	TArray<FVector> PotentialInstanceLocations;
	FLSysFoliageInstanceHash PotentialInstanceHash(7);	// use 128x128 cell size, things like brush radius are typically small
	PotentialInstanceLocations.Empty(DesiredInstances.Num());

	// Reserve space in buckets for a potential instances 
	for (int32 BucketIdx = 0; BucketIdx < NUM_INSTANCE_BUCKETS; ++BucketIdx)
	{
		auto& Bucket = OutPotentialInstances[BucketIdx];
		Bucket.Reserve(DesiredInstances.Num());
	}

	for (const FDesiredLSysInstance& DesiredInst : DesiredInstances)
	{
		FLSysTraceFilterFunc TraceFilterFunc;
		//if (DesiredInst.PlacementMode == EFoliagePlacementMode::Manual && UISettings != nullptr)
		//{
		//	// Enable geometry filters when painting foliage manually
		//	TraceFilterFunc = FFoliagePaintingGeometryFilter(*UISettings);
		//}

		/*if (OverrideGeometryFilter)
		{
			TraceFilterFunc = *OverrideGeometryFilter;
		}
*/
		FHitResult Hit;
		static FName NAME_AddFoliageInstances = FName(TEXT("AddFoliageInstances"));
		//if (ALSystemFoliage::FoliageTrace( Hit, DesiredInst, NAME_AddFoliageInstances, true, TraceFilterFunc))
		//{
		//	float HitWeight = 1.f;

		//	UPrimitiveComponent* InstanceBase = Hit.GetComponent();
		//	check(InstanceBase);

		//	ULevel* TargetLevel = InstanceBase->GetComponentLevel();
		//	// We can paint into new level only if FoliageType is shared
		//	if (!CanPaint(Settings, TargetLevel))
		//	{
		//		continue;
		//	}

		//	const bool bValidInstance = CheckLocationForPotentialInstance(InWorld, Settings, Hit.ImpactPoint, Hit.ImpactNormal, PotentialInstanceLocations, PotentialInstanceHash)
		//		&& VertexMaskCheck(Hit, Settings)
		//		&& LandscapeLayerCheck(Hit, Settings, LocalCache, HitWeight);
		//	if (bValidInstance)
		//	{
		//		const int32 BucketIndex = FMath::RoundToInt(HitWeight * (float)(NUM_INSTANCE_BUCKETS - 1));
		//		OutPotentialInstances[BucketIndex].Add(FPotentialInstance(Hit.ImpactPoint, Hit.ImpactNormal, InstanceBase, HitWeight, DesiredInst));
		//	}
		//}
	}


}





void ALSystemVolume::PostEditImport()
{
	// Make sure that this is the component's spawning volume
	LSystemComponent->SetSpawningVolume(this);
}

bool ALSystemVolume::GetReferencedContentObjects(TArray<UObject*>& Objects) const
{
	Super::GetReferencedContentObjects(Objects);

	if (LSystemComponent && LSystemComponent->LSystemSpawner)
	{
		Objects.Add(LSystemComponent->LSystemSpawner);
	}
	return true;
}

#endif