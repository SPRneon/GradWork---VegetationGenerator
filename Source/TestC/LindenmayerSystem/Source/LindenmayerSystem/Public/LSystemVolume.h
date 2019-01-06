// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/Volume.h"
#include "InstancedLSystemFoliage.h"
#include "LSystemFoliage.h"
#include "LandscapeComponent.h"
#include "LSystemVolume.generated.h"



/**
 * 
 */
struct FDesiredLSysInstance;
class ULSystemComponent;

// Number of buckets for layer weight histogram distribution.
#define NUM_INSTANCE_BUCKETS 10

UCLASS()
class ALSystemVolume : public AVolume
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(Category = Lindenmayer, VisibleAnywhere, BlueprintReadOnly)
	ULSystemComponent* LSystemComponent;

#if WITH_EDITOR
	
	void SpawnLSystemInstances(const TArray<FDesiredLSysInstance>& desiredInsts);
	void SpawnLSystemInstance(const ULSystemFoliageType* Settings, const FLSysFoliageInstance& Instance, UActorComponent* BaseComponent);


private:

	typedef TMap<FName, TMap<ULandscapeComponent*, TArray<uint8> > > LandscapeLayerCacheData;

	void CalculatePotentialInstances(const ULSystemFoliageType* Settings, const TArray<FDesiredLSysInstance>& DesiredInstances, TArray<FLSysPotentialInstance> OutPotentialInstances[NUM_INSTANCE_BUCKETS], LandscapeLayerCacheData* LandscapeLayerCachesPtr = nullptr);
	void AddInstances(const ULSystemFoliageType* Settings, const TArray<FDesiredLSysInstance>& DesiredInstances,const TArray<int32>& ExistingInstanceBuckets, const float Pressure = 1.0f, LandscapeLayerCacheData* LandscapeLayerCachesPtr = nullptr);
	void AddInstance(ALSystemFoliage* LSA);
public:
	


	// UObject interface
	virtual void PostEditImport() override;

	virtual bool GetReferencedContentObjects(TArray<UObject*>& Objects) const override;
#endif
};