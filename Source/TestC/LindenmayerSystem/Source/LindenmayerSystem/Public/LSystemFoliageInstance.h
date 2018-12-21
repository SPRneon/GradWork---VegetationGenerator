//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//
//#include "CoreMinimal.h"
//#include "UObject/ObjectMacros.h"
//#include "LSystemGenerator.h"
//#include "Helpers.h"
//#include "EngineDefines.h"
//#include "LSystemFoliageInstance.generated.h"
//
//
//class UActorComponent;
//struct FLSystemFoliageInstance;
//
//
//#if WITH_PHYSX
//namespace physx
//{
//	class PxRigidActor;
//}
//#endif
//
//UENUM(BlueprintType)
//namespace ESimulationOverlap
//{
//	enum Type
//	{
//		/*Instances overlap with collision*/
//		CollisionOverlap,
//		/*Instances overlap with shade*/
//		ShadeOverlap,
//		/*No overlap*/
//		None
//	};
//}
//
//struct FLSystemFoliageInstance;
//struct FLSystemFoliageOverlap
//{
//	FLSystemFoliageOverlap(FLSystemFoliageInstance* InA, FLSystemFoliageInstance* InB, ESimulationOverlap::Type InType)
//	: A(InA)
//	, B(InB)
//	, OverlapType(InType)
//	{}
//
//	FLSystemFoliageInstance* A;
//	FLSystemFoliageInstance* B;
//	ESimulationOverlap::Type OverlapType;
//};
//
//
//USTRUCT(BlueprintType)
//struct TESTC_API FLSystemFoliageInstance
//{
//public:
//	GENERATED_USTRUCT_BODY()
//	FLSystemFoliageInstance();
//	FLSystemFoliageInstance(const FLSystemFoliageInstance& Other);
//
//	static FLSystemFoliageInstance* Domination(FLSystemFoliageInstance* A, FLSystemFoliageInstance* B, ESimulationOverlap::Type OverlapType);
//
//	float GetMaxRadius() const;
//	float GetShadeRadius() const;
//	float GetCollisionRadius() const;
//
//	bool IsAlive() const { return bAlive; }
//
//	void TerminateInstance();
//
//public:
//	UPROPERTY(Category = Lindenmayer, EditAnywhere, BlueprintReadWrite)
//	FVector Location;
//
//	UPROPERTY()
//	FQuat Rotation;
//
//	UPROPERTY(Category = Lindenmayer, EditAnywhere, BlueprintReadWrite)
//	FVector Normal;
//
//	UPROPERTY(Category = Lindenmayer, EditAnywhere, BlueprintReadWrite)
//	float Age;
//
//	UPROPERTY()
//	float Scale;
//
//	UPROPERTY()
//	ELSystemType Type;
//
//	UPROPERTY()
//	float m_CollisionRadius = 100.f;
//
//	UPROPERTY()
//	float m_ShadeRadius = 100.f;
//
//	UPROPERTY()
//	bool bCanGrowInShade = false;
//
//	bool bBlocker;
//
//	UActorComponent* BaseComponent;
//private:
//	bool bAlive;
//};