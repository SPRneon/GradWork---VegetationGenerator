// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
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
class TESTC_API ALSystemFoliage : public AActor
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
	//Get the max radius
	float GetMaxRadius() const {return (CollisionRadius > ShadeRadius) ? CollisionRadius : ShadeRadius;}
	//Get the collision radius
	float GetCollisionRadius() const {return CollisionRadius;}
	//Get the shade radius
	float GetShadeRadius() const {return ShadeRadius;}
	//Set the transform of the foliage
	FTransform& GetTransform(){return m_Transform;}
	//Get Spreadvariance
	float GetSpreadVariance() const{return SpreadVariance;}
	//Get AverageSpreadvariance
	float GetAvSpreadVariance()const{return AverageSpreadDistance;}
	//Set Spreadvariance
	void SetSpreadVariance(float spreadVar){ SpreadVariance = spreadVar;}
	//Set AverageSpreadvariance
	void SetAvSpreadVariance(float avSpreadVar){ AverageSpreadDistance = avSpreadVar;}




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

	float GetSeedDensitySquared() const { return InitialSeedDensity * InitialSeedDensity; }
	float GetScaleForAge(const float Age) const;
	float GetInitAge(FRandomStream& RandomStream) const;
	float GetNextAge(const float CurrentAge, const int32 NumSteps) const;
	 bool GetSpawnsInShade() const;
	
	//PROCEDURAL
	/** The CollisionRadius determines when two instances overlap. When two instances overlap a winner will be picked based on rules and priority. */
	UPROPERTY(Category=Lindenmayer, EditAnywhere, meta=(Subcategory="Collision", ClampMin="0.0", UIMin="0.0"))
	float CollisionRadius;

	/** The ShadeRadius determines when two instances overlap. If an instance can grow in the shade this radius is ignored.*/
	UPROPERTY(Category=Lindenmayer, EditAnywhere, meta=(Subcategory="Collision", ClampMin="0.0", UIMin="0.0"))
	float ShadeRadius;

	/** The number of times we age the species and spread its seeds. */
	UPROPERTY(Category=Lindenmayer, EditAnywhere, meta=(Subcategory="Clustering", ClampMin="0", UIMin="0"))
	int32 NumSteps;

	// CLUSTERING

	/** Specifies the number of seeds to populate along 10 meters. The number is implicitly squared to cover a 10m x 10m area*/
	UPROPERTY(Category=Lindenmayer, EditAnywhere, meta=(Subcategory="Clustering", ClampMin="0.0", UIMin="0.0"))
	float InitialSeedDensity;

	/** The average distance between the spreading instance and its seeds. For example, a tree with an AverageSpreadDistance 10 will ensure the average distance between the tree and its seeds is 10cm */
	UPROPERTY(Category=Lindenmayer, EditAnywhere, meta=(Subcategory="Clustering", ClampMin="0.0", UIMin="0.0"))
	float AverageSpreadDistance;

	/** Specifies how much seed distance varies from the average. For example, a tree with an AverageSpreadDistance 10 and a SpreadVariance 1 will produce seeds with an average distance of 10cm plus or minus 1cm */
	UPROPERTY(Category=Lindenmayer, EditAnywhere, meta=(Subcategory="Clustering", ClampMin="0.0", UIMin="0.0"))
	float SpreadVariance;

	/** The number of seeds an instance will spread in a single step of the simulation. */
	UPROPERTY(Category=Lindenmayer, EditAnywhere, meta=(Subcategory="Clustering", ClampMin="0", UIMin="0"))
	int32 SeedsPerStep;

	/** The seed that determines placement of initial seeds. */
	UPROPERTY(Category=Lindenmayer, EditAnywhere, meta=(Subcategory="Clustering"))
	int32 DistributionSeed;

	/** The seed that determines placement of initial seeds. */
	UPROPERTY(Category=Lindenmayer, EditAnywhere, meta=(Subcategory="Clustering"))
	float MaxInitialSeedOffset;

	// GROWTH

	/** If true, seeds of this type will ignore shade radius during overlap tests with other types. */
	UPROPERTY(Category=Lindenmayer, EditAnywhere, meta=(Subcategory="Growth"))
	bool bCanGrowInShade;

	/** 
	 * Whether new seeds are spawned exclusively in shade. Occurs in a second pass after all types that do not spawn in shade have been simulated. 
	 * Only valid when CanGrowInShade is true. 
	 */
	UPROPERTY(Category=Lindenmayer, EditAnywhere, meta=(Subcategory="Growth", EditCondition="bCanGrowInShade"))
	bool bSpawnsInShade;

	/** Allows a new seed to be older than 0 when created. New seeds will be randomly assigned an age in the range [0,MaxInitialAge] */
	UPROPERTY(Category=Lindenmayer, EditAnywhere, meta=(Subcategory="Growth", ClampMin="0.0", UIMin="0.0"))
	float MaxInitialAge;

	/** Specifies the oldest a seed can be. After reaching this age the instance will still spread seeds, but will not get any older*/
	UPROPERTY(Category=Lindenmayer, EditAnywhere, meta=(Subcategory="Growth", ClampMin="0.0", UIMin="0.0"))
	float MaxAge;

	/** 
	 * When two instances overlap we must determine which instance to remove. 
	 * The instance with a lower OverlapPriority will be removed. 
	 * In the case where OverlapPriority is the same regular simulation rules apply.
	 */
	UPROPERTY(Category=Lindenmayer, EditAnywhere, meta=(Subcategory="Growth"))
	float OverlapPriority;

	/** The scale range of this type when being procedurally generated. Configured with the Scale Curve. */
	UPROPERTY(Category=Procedural, EditAnywhere, meta = (Subcategory = "Growth", ClampMin = "0.001", UIMin = "0.001"))
	FFloatInterval ProceduralScale;

	/** 
	 * Instance scale factor as a function of normalized age (i.e. Current Age / Max Age).
	 * X = 0 corresponds to Age = 0, X = 1 corresponds to Age = Max Age.
	 * Y = 0 corresponds to Min Scale, Y = 1 corresponds to Max Scale.
	 */
	UPROPERTY(Category = Procedural, EditAnywhere, meta = (Subcategory = "Growth", XAxisName = "Normalized Age", YAxisName = "Scale Factor"))
	FRuntimeFloatCurve ScaleCurve;

	UPROPERTY()
	int32 ChangeCount;

	//Systems
	ULSystemTurtle* m_LSystemTurtle = nullptr;
	ULSystemGenerator* m_LSystemGenerator = nullptr;
};


