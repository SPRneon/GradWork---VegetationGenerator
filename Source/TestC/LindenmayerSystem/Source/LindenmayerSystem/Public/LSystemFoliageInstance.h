//// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "EngineDefines.h"
#include "LSystemFoliageInstance.generated.h"

class UActorComponent;
class ULSystemFoliageType;
struct FLSysFolInstance;

#if WITH_PHYSX
namespace physx
{
	class PxRigidActor;
}
#endif

UENUM(BlueprintType)
namespace ELSysSimulationOverlap
{
	enum Type
	{
		/*Instances overlap with collision*/
		CollisionOverlap,
		/*Instances overlap with shade*/
		ShadeOverlap,
		/*No overlap*/
		None
	};
}

UENUM(BlueprintType)
namespace ELSysSimulationQuery
{
	enum Type
	{
		/*Instances overlap with collision*/
		CollisionOverlap = 1,
		/*Instances overlap with shade*/
		ShadeOverlap = 2,
		/*any overlap*/
		AnyOverlap = 3
	};
}

struct FLSysFoliageOverlap;
struct FLSysFoliageOverlap
{
	FLSysFoliageOverlap(FLSysFolInstance* InA, FLSysFolInstance* InB, ELSysSimulationOverlap::Type InType)
	: A(InA)
	, B(InB)
	, OverlapType(InType)
	{}

	FLSysFolInstance* A;
	FLSysFolInstance* B;
	ELSysSimulationOverlap::Type OverlapType;
};

USTRUCT(BlueprintType)
struct FLSysFolInstance
{
public:
	GENERATED_USTRUCT_BODY()
	FLSysFolInstance();
	FLSysFolInstance(const FLSysFolInstance& Other);

	static FLSysFolInstance* Domination(FLSysFolInstance* A, FLSysFolInstance* B, ELSysSimulationOverlap::Type OverlapType);

	float GetMaxRadius() const;
	float GetShadeRadius() const;
	float GetCollisionRadius() const;

	bool IsAlive() const { return bAlive; }

	void TerminateInstance();

public:
	UPROPERTY(Category = ProceduralFoliageInstance, EditAnywhere, BlueprintReadWrite)
	FVector Location;

	UPROPERTY()
	FQuat Rotation;

	UPROPERTY(Category = ProceduralFoliageInstance, EditAnywhere, BlueprintReadWrite)
	FVector Normal;

	UPROPERTY(Category = ProceduralFoliageInstance, EditAnywhere, BlueprintReadWrite)
	float Age;

	UPROPERTY()
	float Scale;

	UPROPERTY()
	const ULSystemFoliageType* Type;


	bool bBlocker;

	UActorComponent* BaseComponent;
private:
	bool bAlive;
};
