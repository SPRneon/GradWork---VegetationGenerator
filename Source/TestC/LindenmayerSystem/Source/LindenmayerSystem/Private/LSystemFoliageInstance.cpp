

#include "LSystemFoliageInstance.h"
#include "LindemayerFoliageType.h"

FLSysFolInstance::FLSysFolInstance()
: Location(ForceInit)
, Rotation(ForceInit)
, Normal(ForceInit)
, Age(0)
, Scale(1)
, Type(nullptr)
, bBlocker(false)
, BaseComponent(nullptr)
, bAlive(true)
{
}

FLSysFolInstance::FLSysFolInstance(const FLSysFolInstance& Other)
: Location(Other.Location)
, Rotation(Other.Rotation)
, Normal(Other.Normal)
, Age(Other.Age)
, Scale(Other.Scale)
, Type(Other.Type)
, bBlocker(Other.bBlocker)
, BaseComponent(Other.BaseComponent)
, bAlive(Other.bAlive)
{

}

FLSysFolInstance* GetLessFit(FLSysFolInstance* A, FLSysFolInstance* B)
{
	//Blocker is used for culling instances when we overlap tiles. It always wins
	if (A->bBlocker){ return B; }
	if (B->bBlocker){ return A; }
	
	//we look at priority, then age, then radius
	if (A->Type->OverlapPriority == B->Type->OverlapPriority)
	{
		if (A->Age == B->Age)
		{
			return A->Scale < B->Scale ? A : B;
		}
		else
		{
			return A->Age < B->Age ? A : B;
		}
	}
	else
	{
		return A->Type->OverlapPriority < B->Type->OverlapPriority ? A : B;
	}
}


FLSysFolInstance* FLSysFolInstance::Domination(FLSysFolInstance* A, FLSysFolInstance* B, ELSysSimulationOverlap::Type OverlapType)
{
	const ULSystemFoliageType* AType = A->Type;
	const ULSystemFoliageType* BType = B->Type;

	FLSysFolInstance* Dominated = GetLessFit(A, B);

	if (OverlapType == ELSysSimulationOverlap::ShadeOverlap && Dominated->Type->bCanGrowInShade)
	{
		return nullptr;
	}

	return Dominated;
}

float FLSysFolInstance::GetMaxRadius() const
{
	const float CollisionRadius = GetCollisionRadius();
	const float ShadeRadius = GetShadeRadius();
	return FMath::Max(CollisionRadius, ShadeRadius);
}

float FLSysFolInstance::GetShadeRadius() const
{
	const float ShadeRadius = Type->ShadeRadius * Scale;
	return ShadeRadius;
}

float FLSysFolInstance::GetCollisionRadius() const
{
	const float CollisionRadius = Type->CollisionRadius * Scale;
	return CollisionRadius;
}

void FLSysFolInstance::TerminateInstance()
{
	bAlive = false;
}