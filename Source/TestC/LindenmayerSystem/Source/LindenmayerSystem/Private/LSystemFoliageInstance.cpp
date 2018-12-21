//// Fill out your copyright notice in the Description page of Project Settings.
//
//#include "LSystemFoliageInstance.h"
//#include "LSystemGenerator.h"
//#include "Helpers.h"
//
//FLSystemFoliageInstance::FLSystemFoliageInstance()
//: Location(ForceInit)
//, Rotation(ForceInit)
//, Normal(ForceInit)
//, Age(0)
//, Scale(1)
//, Type(ELSystemType::PLANT)
//, bBlocker(false)
//, BaseComponent(nullptr)
//, bAlive(true)
//{
//}
//
//FLSystemFoliageInstance::FLSystemFoliageInstance(const FLSystemFoliageInstance& Other)
//: Location(Other.Location)
//, Rotation(Other.Rotation)
//, Normal(Other.Normal)
//, Age(Other.Age)
//, Scale(Other.Scale)
//, Type(Other.Type)
//, bBlocker(Other.bBlocker)
//, BaseComponent(Other.BaseComponent)
//, bAlive(Other.bAlive)
//{
//
//}
//
//
//FLSystemFoliageInstance* GetLessFit(FLSystemFoliageInstance* A, FLSystemFoliageInstance* B)
//{
//	//Blocker is used for culling instances when we overlap tiles. It always wins
//	if (A->bBlocker){ return B; }
//	if (B->bBlocker){ return A; }
//	
//	//we look at age, then radius
//
//		if (A->Age == B->Age)
//		{
//			return A->Scale < B->Scale ? A : B;
//		}
//		else
//		{
//			return A->Age < B->Age ? A : B;
//		}
//	
//}
//
//FLSystemFoliageInstance* FLSystemFoliageInstance::Domination(FLSystemFoliageInstance* A, FLSystemFoliageInstance* B, ESimulationOverlap::Type OverlapType)
//{
//	FLSystemFoliageInstance* Dominated = GetLessFit(A, B);
//
//	if (OverlapType == ESimulationOverlap::ShadeOverlap && Dominated->bCanGrowInShade)
//	{
//		return nullptr;
//	}
//
//	return Dominated;
//}
//
//float FLSystemFoliageInstance::GetMaxRadius() const
//{
//	const float CollisionRadius = GetCollisionRadius();
//	const float ShadeRadius = GetShadeRadius();
//	return FMath::Max(CollisionRadius, ShadeRadius);
//}
//
//float FLSystemFoliageInstance::GetShadeRadius() const
//{
//	const float ShadeRadius = m_ShadeRadius * Scale;
//	return ShadeRadius;
//}
//
//float FLSystemFoliageInstance::GetCollisionRadius() const
//{
//	const float CollisionRadius = m_CollisionRadius * Scale;
//	return CollisionRadius;
//}
//
//void FLSystemFoliageInstance::TerminateInstance()
//{
//	bAlive = false;
//}