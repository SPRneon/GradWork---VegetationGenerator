#pragma once
#include "CoreMinimal.h"
#include "Misc/Guid.h"

typedef int32 FLSysFoliageInstanceBaseId;

class UActorComponent;
class ULSystemFoliageType;
class UPrimitiveComponent;

enum ELSysFoliageInstanceFlags
{
	LSYS_AlignToNormal = 0x00000001,
	LSYS_NoRandomYaw = 0x00000002,
	LSYS_Readjusted = 0x00000004,
	LSYS_InstanceDeleted = 0x00000008,	// Used only for migration from pre-HierarchicalISM foliage.
};

struct FLSysFoliageInstancePlacementInfo
{
	FVector Location;
	FRotator Rotation;
	FRotator PreAlignRotation;
	FVector DrawScale3D;
	float ZOffset;
	uint32 Flags;

	FLSysFoliageInstancePlacementInfo()
		: Location(0.f, 0.f, 0.f)
		, Rotation(0, 0, 0)
		, PreAlignRotation(0, 0, 0)
		, DrawScale3D(1.f, 1.f, 1.f)
		, ZOffset(0.f)
		, Flags(0)
	{}
};

struct FLSysFoliageInstance_Deprecated : public FLSysFoliageInstancePlacementInfo
{
	UActorComponent* Base;
	FGuid ProceduralGuid;
	friend FArchive& operator<<(FArchive& Ar, FLSysFoliageInstance_Deprecated& Instance);
};

struct FLSysFoliageInstance : public FLSysFoliageInstancePlacementInfo
{
	// ID of base this instance was painted on
	FLSysFoliageInstanceBaseId BaseId;

	FGuid ProceduralGuid;

	FLSysFoliageInstance()
		: BaseId(0)
	{}

	friend FArchive& operator<<(FArchive& Ar, FLSysFoliageInstance& Instance);

	FTransform GetInstanceWorldTransform() const
	{
		return FTransform(Rotation, Location, DrawScale3D);
	}

	void AlignToNormal(const FVector& InNormal, float AlignMaxAngle = 0.f)
	{
		Flags |= LSYS_AlignToNormal;

		FRotator AlignRotation = InNormal.Rotation();
		// Static meshes are authored along the vertical axis rather than the X axis, so we add 90 degrees to the static mesh's Pitch.
		AlignRotation.Pitch -= 90.f;
		// Clamp its value inside +/- one rotation
		AlignRotation.Pitch = FRotator::NormalizeAxis(AlignRotation.Pitch);

		// limit the maximum pitch angle if it's > 0.
		if (AlignMaxAngle > 0.f)
		{
			int32 MaxPitch = AlignMaxAngle;
			if (AlignRotation.Pitch > MaxPitch)
			{
				AlignRotation.Pitch = MaxPitch;
			}
			else if (AlignRotation.Pitch < -MaxPitch)
			{
				AlignRotation.Pitch = -MaxPitch;
			}
		}

		PreAlignRotation = Rotation;
		Rotation = FRotator(FQuat(AlignRotation) * FQuat(Rotation));
	}
};

#if WITH_EDITORONLY_DATA
//
// FFoliageInstanceHash
//

#define FOLIAGE_HASH_CELL_BITS 9	// 512x512 grid

struct FLSysFoliageInstanceHash
{
private:
	const int32 HashCellBits;
	TMap<uint64, TSet<int32>> CellMap;

	uint64 MakeKey(int32 CellX, int32 CellY) const
	{
		return ((uint64)(*(uint32*)(&CellX)) << 32) | (*(uint32*)(&CellY) & 0xffffffff);
	}

	uint64 MakeKey(const FVector& Location) const
	{
		return  MakeKey(FMath::FloorToInt(Location.X) >> HashCellBits, FMath::FloorToInt(Location.Y) >> HashCellBits);
	}

public:
	FLSysFoliageInstanceHash(int32 InHashCellBits = FOLIAGE_HASH_CELL_BITS)
		: HashCellBits(InHashCellBits)
	{}

	void InsertInstance(const FVector& InstanceLocation, int32 InstanceIndex)
	{
		uint64 Key = MakeKey(InstanceLocation);

		CellMap.FindOrAdd(Key).Add(InstanceIndex);
	}

	void RemoveInstance(const FVector& InstanceLocation, int32 InstanceIndex)
	{
		uint64 Key = MakeKey(InstanceLocation);

		int32 RemoveCount = CellMap.FindChecked(Key).Remove(InstanceIndex);
		check(RemoveCount == 1);
	}

	void GetInstancesOverlappingBox(const FBox& InBox, TArray<int32>& OutInstanceIndices) const
	{
		int32 MinX = FMath::FloorToInt(InBox.Min.X) >> HashCellBits;
		int32 MinY = FMath::FloorToInt(InBox.Min.Y) >> HashCellBits;
		int32 MaxX = FMath::FloorToInt(InBox.Max.X) >> HashCellBits;
		int32 MaxY = FMath::FloorToInt(InBox.Max.Y) >> HashCellBits;

		for (int32 y = MinY; y <= MaxY; y++)
		{
			for (int32 x = MinX; x <= MaxX; x++)
			{
				uint64 Key = MakeKey(x, y);
				auto* SetPtr = CellMap.Find(Key);
				if (SetPtr)
				{
					OutInstanceIndices.Append(SetPtr->Array());
				}
			}
		}
	}

	TArray<int32> GetInstancesOverlappingBox(const FBox& InBox) const
	{
		TArray<int32> Result;
		GetInstancesOverlappingBox(InBox, Result);
		return Result;
	}

#if UE_BUILD_DEBUG
	void CheckInstanceCount(int32 InCount) const
	{
		int32 HashCount = 0;
		for (const auto& Pair : CellMap)
		{
			HashCount += Pair.Value.Num();
		}

		check(HashCount == InCount);
	}
#endif

	void Empty()
	{
		CellMap.Empty();
	}

	friend FArchive& operator<<(FArchive& Ar, FLSysFoliageInstanceHash& Hash)
	{
		Ar << Hash.CellMap;
		return Ar;
	}
};
#endif

namespace ELSysPlacementMode
{
	enum Type
	{
		Manual = 0,
		Procedural = 1,
	};

}

struct FDesiredLSysInstance
{
	FDesiredLSysInstance()
		: FoliageType(nullptr)
		, StartTrace(ForceInit)
		, EndTrace(ForceInit)
		, Rotation(ForceInit)
		, TraceRadius(0.f)
		, Age(0.f)
		, PlacementMode(ELSysPlacementMode::Manual)
	{

	}

	FDesiredLSysInstance(const FVector& InStartTrace, const FVector& InEndTrace, const float InTraceRadius = 0.f)
		: FoliageType(nullptr)
		, StartTrace(InStartTrace)
		, EndTrace(InEndTrace)
		, Rotation(ForceInit)
		, TraceRadius(InTraceRadius)
		, Age(0.f)
		, PlacementMode(ELSysPlacementMode::Manual)
	{
	}

	const ULSystemFoliageType* FoliageType;
	FGuid ProceduralGuid;
	FVector StartTrace;
	FVector EndTrace;
	FQuat Rotation;
	float TraceRadius;
	float Age;
	const struct FBodyInstance* ProceduralVolumeBodyInstance;
	ELSysPlacementMode::Type PlacementMode;
};


#if WITH_EDITOR
// Struct to hold potential instances we've sampled
struct FLSysPotentialInstance
{
	FVector HitLocation;
	FVector HitNormal;
	UPrimitiveComponent* HitComponent;
	float HitWeight;
	FDesiredLSysInstance DesiredInstance;

	FLSysPotentialInstance(FVector InHitLocation, FVector InHitNormal, UPrimitiveComponent* InHitComponent, float InHitWeight, const FDesiredLSysInstance& InDesiredInstance = FDesiredLSysInstance());
	bool PlaceInstance(const UWorld* InWorld, const ULSystemFoliageType* Settings, FLSysFoliageInstance& Inst, bool bSkipCollision = false);
};
#endif