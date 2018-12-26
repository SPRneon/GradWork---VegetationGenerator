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


class ULSystemTurtle;
class UTree;
class ULSystemGenerator;
class ALSystemFoliage;



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
	GENERATED_BODY()	
public:	
	// Sets default values for this actor's properties
	ALSystemFoliage();

	

	friend class ULSystemTile;

	void Initialize(int age, ELSystemType type);
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
	int m_Generation = 1;
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

	bool bBlocker;


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


private:
	
	void CreateFoliageTypeInstance();
	void ResimulateTree();
	void CreateSplineMeshComponents(USplineComponent* spline, float width);
	void CreateLeafMeshes(UTree* tree, USplineComponent* spline);
	void ConvertMeshes();

	//TODO
	//CALCULATE SHADES AND BOUNDS

	//Data	
	FTransform m_Transform;
	FVector2D m_Angle = FVector2D(0.f,0.f);
	float m_GrowthDist = 0.0f;
	UTree* m_RootTree = nullptr;
	bool m_IsAlive = false;



	//Systems
	ULSystemTurtle* m_LSystemTurtle = nullptr;
	ULSystemGenerator* m_LSystemGenerator = nullptr;
};

