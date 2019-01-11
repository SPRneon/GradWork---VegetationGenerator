// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "LindemayerFoliageType.h"
#include "Helpers.h"
#include "Curves/CurveFloat.h"

//#include "LSystemFoliageInstance.h"
#include "LSystemFoliage.generated.h"


struct FDesiredLSysInstance;
struct FLSysPotentialInstance;
struct FLSysFoliageInstance;
class ULSystemTurtle;
class UTree;
class ULSystemGenerator;
class ALSystemFoliage;
class UInstancedStaticMeshComponent;

typedef TFunction<bool(const UPrimitiveComponent*)> FLSysTraceFilterFunc;

#if WITH_PHYSX
namespace physx
{
	class PxRigidActor;
}
#endif

UENUM(BlueprintType)
namespace ESimulationOverlap
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

class ALSystemFoliage;
struct FLSystemFoliageOverlap
{
	FLSystemFoliageOverlap(ALSystemFoliage* InA, ALSystemFoliage* InB, ESimulationOverlap::Type InType)
	: A(InA)
	, B(InB)
	, OverlapType(InType)
	{}

	ALSystemFoliage* A;
	ALSystemFoliage* B;
	ESimulationOverlap::Type OverlapType;
};

UCLASS()
class ALSystemFoliage : public AActor
{
public:
	GENERATED_BODY()	
public:	
	// Sets default values for this actor's properties
	ALSystemFoliage();

	

	friend class ULSystemTile;


	void Initialize(int age, ELSystemType type);
	void Initialize(const FLSysPotentialInstance& Inst);
	TArray<USplineComponent*>& GetSplineComponentArray();
	void CreateSplines(UTree* tree);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostActorCreated() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostEditMove(bool bFinished) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	


public:
	//Components
	UPROPERTY()
	USceneComponent* Root = nullptr;

	UPROPERTY(Category = Lindenmayer, EditAnywhere)
	const ULSystemFoliageType* Type;

	//Meshes
	UPROPERTY(EditAnywhere)
	UStaticMesh* m_Mesh = nullptr;
	
	UPROPERTY(EditAnywhere)
	UStaticMesh* m_SplineMesh = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMesh* m_LeafMesh = nullptr;

	//Data
	UPROPERTY(EditAnywhere)
	int m_Generation = 3;
	UPROPERTY(VisibleAnywhere)
	ELSystemType m_Type = ELSystemType::PLANT;
	UPROPERTY(VisibleAnywhere)
	FString m_LString = "";

	
	

	/**Spline component*/
     UPROPERTY(EditAnywhere, Category = Lindenmayer)
     TArray<USplineComponent*> m_SplineComponents;
	/**SplineMesh component*/
     UPROPERTY(EditAnywhere, Category = Lindenmayer)
     TArray<USplineMeshComponent*> m_SplineMeshComponents;
	/**Spline component*/
     UPROPERTY(EditAnywhere, Category = Lindenmayer)
     UInstancedStaticMeshComponent* m_LeafMeshComponents;

	bool bBlocker;

public:
#if WITH_EDITOR
	static bool FoliageTrace(const UWorld* InWorld, FHitResult& OutHit, const FDesiredLSysInstance& DesiredInstance, FName InTraceTag = NAME_None, bool InbReturnFaceIndex = false, const FLSysTraceFilterFunc& FilterFunc = FLSysTraceFilterFunc());
	static bool CheckCollisionWithWorld(const UWorld* InWorld, const ULSystemFoliageType* Settings, const FLSysFoliageInstance& Inst, const FVector& HitNormal, const FVector& HitLocation, UPrimitiveComponent* HitComponent);

#endif



	//Check which foliage is dominating the other
	static ALSystemFoliage* Domination(ALSystemFoliage* A, ALSystemFoliage* B, ESimulationOverlap::Type OverlapType);

	//Age the foliage
	void AgeFoliage(int newAge = 1);
	//Sets m_IsAlive to false
	void TerminateInstance(){ m_IsAlive = false;}
	//Check if this is alive
	bool IsAlive() const {return m_IsAlive;}
	//Returns the componentboundingbox of this ALSystemfoliage
	FBox GetBounds() const;
	//Get the current Gen
	int GetGen()const {return m_Generation;}
	//Get the current type
	ELSystemType GetType() const {return  m_Type;}
	//Resimulate the tree
	void ResimulateTree();
private:
	
	void CreateFoliageTypeInstance();
	void SmoothSplines();
	void CreateSplineMeshComponents(USplineComponent* spline, UTree* branch);
	void CreateLeafMeshes(UTree* tree, USplineComponent* spline);
	void CreateEndLeaves(UTree* tree, USplineComponent* spline);
	void ConvertMeshes();

	//TODO
	//CALCULATE SHADES AND BOUNDS

	//Data	
	FTransform m_Transform;
	FVector2D m_Angle = FVector2D(0.f,0.f);
	float m_GrowthDist = 0.0f;
	UTree* m_RootTree = nullptr;
	bool m_IsAlive = false;

	TMap<UTree*, USplineComponent*> m_SplineMap;
	
	//Systems
	ULSystemTurtle* m_LSystemTurtle = nullptr;
	ULSystemGenerator* m_LSystemGenerator = nullptr;
};


