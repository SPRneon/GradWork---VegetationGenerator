// Fill out your copyright notice in the Description page of Project Settings.

#include "LindenmayerSystem/Source/LindenmayerSystem/Public/LindemayerFoliageType.h"
#include "UObjectIterator.h"
#include "LindenmayerSystem/Source/LindenmayerSystem/Public/LSystemFoliage.h"
#include "HAL/IConsoleManager.h"
#include "WorldCollision.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/Level.h"
#include "GameFramework/Actor.h"
#include "CollisionQueryParams.h"
#include "WorldCollision.h"
#include "Engine/CollisionProfile.h"

#define LOCTEXT_NAMESPACE "LSystemFoliageType"

ULSystemFoliageType::ULSystemFoliageType(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	Density = 100.0f;
	Radius = 0.0f;
	AlignToNormal = true;
	RandomYaw = true;
	Scaling = ELSystemScaling::Uniform;
	ScaleX.Min = 1.0f;
	ScaleY.Min = 1.0f;
	ScaleZ.Min = 1.0f;
	ScaleX.Max = 1.0f;
	ScaleY.Max = 1.0f;
	ScaleZ.Max = 1.0f;
	AlignMaxAngle = 0.0f;
	RandomPitchAngle = 0.0f;
	GroundSlopeAngle.Min = 0.0f;
	GroundSlopeAngle.Max = 45.0f;
	Height.Min = -262144.0f;
	Height.Max = 262144.0f;
	ZOffset.Min = 0.0f;
	ZOffset.Max = 0.0f;

	MinimumLayerWeight = 0.5f;

	DensityAdjustmentFactor = 1.0f;
	CollisionWithWorld = false;
	CollisionScale = FVector(0.9f, 0.9f, 0.9f);



	BodyInstance.SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

	/** Ecosystem settings*/
	AverageSpreadDistance = 50;
	SpreadVariance = 150;
	bCanGrowInShade = false;
	bSpawnsInShade = false;
	SeedsPerStep = 3;
	OverlapPriority = 0.f;
	NumSteps = 3;
	ProceduralScale = FFloatInterval(1.f, 3.f);
	ChangeCount = 0;
	InitialSeedDensity = 1.f;
	CollisionRadius = 100.f;
	ShadeRadius = 100.f;
	MaxInitialAge = 0.f;
	MaxAge = 10.f;

	FRichCurve* Curve = ScaleCurve.GetRichCurve();
	Curve->AddKey(0.f, 0.f);
	Curve->AddKey(1.f, 1.f);

	UpdateGuid = FGuid::NewGuid();

	bEnableDensityScaling = false;

#if WITH_EDITORONLY_DATA
	// Deprecated since FFoliageCustomVersion::FoliageTypeCustomization
	ScaleMinX_DEPRECATED = 1.0f;
	ScaleMinY_DEPRECATED = 1.0f;
	ScaleMinZ_DEPRECATED = 1.0f;
	ScaleMaxX_DEPRECATED = 1.0f;
	ScaleMaxY_DEPRECATED = 1.0f;
	ScaleMaxZ_DEPRECATED = 1.0f;
	HeightMin_DEPRECATED = -262144.0f;
	HeightMax_DEPRECATED = 262144.0f;
	ZOffsetMin_DEPRECATED = 0.0f;
	ZOffsetMax_DEPRECATED = 0.0f;
	UniformScale_DEPRECATED = true;
	GroundSlope_DEPRECATED = 45.0f;

	// Deprecated since FFoliageCustomVersion::FoliageTypeProceduralScaleAndShade
	MinScale_DEPRECATED = 1.f;
	MaxScale_DEPRECATED = 3.f;

#endif// WITH_EDITORONLY_DATA
}

bool ULSystemFoliageType::IsNotAssetOrBlueprint() const
{
	return IsAsset() == false && Cast<UBlueprint>(GetClass()->ClassGeneratedBy) == nullptr;
}

FVector ULSystemFoliageType::GetRandomScale() const
{
	FVector Result(1.0f);
	float LockRand = 0.0f;

	switch (Scaling)
	{
	case ELSystemScaling::Uniform:
		Result.X = ScaleX.Interpolate(FMath::FRand());
		Result.Y = Result.X;
		Result.Z = Result.X;
		break;

	case ELSystemScaling::Free:
		Result.X = ScaleX.Interpolate(FMath::FRand());
		Result.Y = ScaleY.Interpolate(FMath::FRand());
		Result.Z = ScaleZ.Interpolate(FMath::FRand());
		break;

	case ELSystemScaling::LockXY:
		LockRand = FMath::FRand();
		Result.X = ScaleX.Interpolate(LockRand);
		Result.Y = ScaleY.Interpolate(LockRand);
		Result.Z = ScaleZ.Interpolate(FMath::FRand());
		break;

	case ELSystemScaling::LockXZ:
		LockRand = FMath::FRand();
		Result.X = ScaleX.Interpolate(LockRand);
		Result.Y = ScaleY.Interpolate(FMath::FRand());
		Result.Z = ScaleZ.Interpolate(LockRand);

	case ELSystemScaling::LockYZ:
		LockRand = FMath::FRand();
		Result.X = ScaleX.Interpolate(FMath::FRand());
		Result.Y = ScaleY.Interpolate(LockRand);
		Result.Z = ScaleZ.Interpolate(LockRand);
	}

	return Result;
}
float ULSystemFoliageType::GetMaxRadius() const
{
	return FMath::Max(CollisionRadius, ShadeRadius);
}

float ULSystemFoliageType::GetScaleForAge(const float Age) const
{
	const FRichCurve* Curve = ScaleCurve.GetRichCurveConst();
	const float Time = FMath::Clamp(MaxAge == 0 ? 1.f : Age / MaxAge, 0.f, 1.f);
	const float Scale = Curve->Eval(Time);
	return ProceduralScale.Min + ProceduralScale.Size() * Scale;
}

float ULSystemFoliageType::GetInitAge(FRandomStream& RandomStream) const
{
	return RandomStream.FRandRange(0,MaxInitialAge);
}

float ULSystemFoliageType::GetNextAge(const float CurrentAge, const int32 NumSteps) const
{
	float NewAge = CurrentAge;
	for (int32 Count = 0; Count < NumSteps; ++Count)
	{
		const float GrowAge = NewAge + 1;
		if (GrowAge <= MaxAge)
		{
			NewAge = GrowAge;
		}
		else
		{
			break;
		}
	}

	return NewAge;
}

bool ULSystemFoliageType::GetSpawnsInShade() const
{
	return bCanGrowInShade && bSpawnsInShade;
}

#if WITH_EDITOR
void ULSystemFoliageType::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Ensure that OverriddenLightMapRes is a factor of 4
	//OverriddenLightMapRes = OverriddenLightMapRes > 4 ? OverriddenLightMapRes + 3 & ~3 : 4;
	++ChangeCount;

	UpdateGuid = FGuid::NewGuid();

	//@todo: move this into FoliageType_InstancedStaticMesh
	// Check to see if the mesh is what changed
	//TODO check if spline is what changed instead of instanced mesh
	//const bool bMeshChanged = PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(ALSystemFoliage, Mesh);

	// Notify any currently-loaded InstancedFoliageActors
	//TODO Get Actor aware of change
	//if (IsFoliageReallocationRequiredForPropertyChange(PropertyChangedEvent))
	//{
	//	for (TObjectIterator<ALSystemFoliage> It(RF_ClassDefaultObject, /** bIncludeDerivedClasses */ true, /** InternalExcludeFalgs */ EInternalObjectFlags::PendingKill); It; ++It)
	//	{
	//		if (It->GetWorld() != nullptr)
	//		{
	//			It->NotifyFoliageTypeChanged(this, bMeshChanged);
	//		}
	//	}
	//}
}

void ULSystemFoliageType::PreEditChange(UProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);

	//if (PropertyAboutToChange && PropertyAboutToChange->GetFName() == GET_MEMBER_NAME_CHECKED(UFoliageType_InstancedStaticMesh, Mesh))
	//{
	//	for (TObjectIterator<ALSystemFoliage> It(RF_ClassDefaultObject, /** bIncludeDerivedClasses */ true, /** InternalExcludeFalgs */ EInternalObjectFlags::PendingKill); It; ++It)
	//	{
	//		//TODO Get notify change in Actor
	//		//It->NotifyFoliageTypeWillChange(this, true);
	//	}
	//}
}

FName ULSystemFoliageType::GetDisplayFName() const
{
	FName DisplayFName;

	if (IsAsset())
	{
		DisplayFName = GetFName();
	}
	else if (UBlueprint* FoliageTypeBP = Cast<UBlueprint>(GetClass()->ClassGeneratedBy))
	{
		DisplayFName = FoliageTypeBP->GetFName();
	}
	//else if (UStaticMesh* StaticMesh = GetStaticMesh())
	//{
	//	DisplayFName = StaticMesh->GetFName();
	//}

	return DisplayFName;
}

#endif //with editor



#undef LOCTEXT_NAMESPACE
