// Fill out your copyright notice in the Description page of Project Settings.

#include "LSystemVolume.h"
#include "Components/BrushComponent.h"
#include "Engine/World.h"
#include "LSystemComponent.h"
#include "LSystemFoliage.h"
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


void ALSystemVolume::Destroyed()
{
	for(ALSystemFoliage* LSF: FoliageActors)
	{
		LSF->Destroy();
	}
}


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

void ALSystemVolume::SpawnLSystemInstance(const ULSystemFoliageType * Settings, const FLSysFoliageInstance & Instance, const FLSysPotentialInstance& desiredInst)
{
	// We always spawn instances in base component level
	ULevel* TargetLevel =  GetWorld()->GetCurrentLevel();
	//CurrentFoliageTraceBrushAffectedLevels.AddUnique(TargetLevel);
	FActorSpawnParameters SpawnParams;
	SpawnParams.OverrideLevel = TargetLevel;
	SpawnParams.Owner = this;	
	
	
	ALSystemFoliage* LSA  = GetWorld()->SpawnActor<ALSystemFoliage>(SpawnParams);
	LSA->Initialize(desiredInst);
	LSA->SetActorTransform(Instance.GetInstanceWorldTransform());
	LSA->AttachToActor(this,FAttachmentTransformRules::KeepWorldTransform);
	FoliageActors.Add(LSA);
	

	

}

void ALSystemVolume::AddInstances(const ULSystemFoliageType* Settings, const TArray<FDesiredLSysInstance>& DesiredInstances, const TArray<int32>& ExistingInstanceBuckets, const float Pressure, LandscapeLayerCacheData* LandscapeLayerCachesPtr)
{
	if (DesiredInstances.Num() == 0)
	{
		return;
	}

	TArray<FLSysPotentialInstance> PotentialInstanceBuckets[NUM_INSTANCE_BUCKETS];
	
	CalculatePotentialInstances(Settings, DesiredInstances, PotentialInstanceBuckets);
	

	for (int32 BucketIdx = 0; BucketIdx < NUM_INSTANCE_BUCKETS; BucketIdx++)
	{
		TArray<FLSysPotentialInstance>& PotentialInstances = PotentialInstanceBuckets[BucketIdx];
		float BucketFraction = (float)(BucketIdx + 1) / (float)NUM_INSTANCE_BUCKETS;

		// We use the number that actually succeeded in placement (due to parameters) as the target
		// for the number that should be in the brush region.
		const int32 BucketOffset = (ExistingInstanceBuckets.Num() ? ExistingInstanceBuckets[BucketIdx] : 0);
		int32 AdditionalInstances = FMath::Clamp<int32>(FMath::RoundToInt(BucketFraction * (float)(PotentialInstances.Num() - BucketOffset) * Pressure), 0, PotentialInstances.Num());
		
		
		for (int32 Idx = 0; Idx < AdditionalInstances; Idx++)
		{
			FLSysPotentialInstance& PotentialInstance = PotentialInstances[Idx];
			FLSysFoliageInstance Inst;
			if (PotentialInstance.PlaceInstance(GetWorld(), Settings, Inst))
			{
				Inst.ProceduralGuid = PotentialInstance.DesiredInstance.ProceduralGuid;

				SpawnLSystemInstance(Settings, Inst, PotentialInstance);
			}
		}
	}
}

void ALSystemVolume::AddInstance(ALSystemFoliage * LSA)
{
}

bool ALSystemVolume::CheckLocationForPotentialInstance(const UWorld* InWorld, const ULSystemFoliageType* Settings, const FVector& Location, const FVector& Normal, TArray<FVector>& PotentialInstanceLocations, FLSysFoliageInstanceHash& PotentialInstanceHash)
{
	//todo
	return true;
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
		
		FHitResult Hit;
		static FName NAME_AddFoliageInstances = FName(TEXT("AddFoliageInstances"));

		if (ALSystemFoliage::FoliageTrace(GetWorld(), Hit, DesiredInst, NAME_AddFoliageInstances, true, TraceFilterFunc))
		{
			float HitWeight = 1.f;

			UPrimitiveComponent* InstanceBase = Hit.GetComponent();
			check(InstanceBase);

			ULevel* TargetLevel = InstanceBase->GetComponentLevel();
			// We can paint into new level only if FoliageType is shared
			

			const bool bValidInstance = CheckLocationForPotentialInstance(GetWorld(), Settings, Hit.ImpactPoint, Hit.ImpactNormal, PotentialInstanceLocations, PotentialInstanceHash)
				//&& VertexMaskCheck(Hit, Settings)
				//&& LandscapeLayerCheck(Hit, Settings, LocalCache, HitWeight)
			;
			if (bValidInstance)
			{
				const int32 BucketIndex = FMath::RoundToInt(HitWeight * (float)(NUM_INSTANCE_BUCKETS - 1));
				OutPotentialInstances[BucketIndex].Add(FLSysPotentialInstance(Hit.ImpactPoint, Hit.ImpactNormal, InstanceBase, HitWeight, DesiredInst));
			}
		}
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