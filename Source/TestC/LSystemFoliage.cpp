// Fill out your copyright notice in the Description page of Project Settings.

#include "LSystemFoliage.h"
#include "LSystemGenerator.h"
//#include "LSystemTree.h"
#include "LSytemTurtle.h"
#include "LSystemTree.h"
#include "LSystemSplineGenerator.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Runtime/Foliage/Public/InstancedFoliageActor.h"
#include "EngineUtils.h"
#include "ConstructorHelpers.h"

// Sets default values
ALSystemFoliage::ALSystemFoliage()
{
	m_Transform = GetTransform();

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	const ConstructorHelpers::FObjectFinder<UStaticMesh> splineMesh(TEXT("/Game/VegetationGenerator/Branch_S.Branch_S"));
	m_SplineMesh = splineMesh.Object;

	
	const ConstructorHelpers::FObjectFinder<UStaticMesh> LeafMeshObj(TEXT("/Game/VegetationGenerator/Leaf.Leaf"));
	m_LeafMesh = LeafMeshObj.Object;	
	m_IsAlive = true;	
}

void ALSystemFoliage::Initialize(int age, ELSystemType type)
{
	m_Generation = age;
	m_Type = type;
	m_LString = ULSystemGenerator::GenerateLString(m_Type, m_Generation);	
	m_RootTree = ULSystemTurtle::IterateTurtle(m_LString,Root,m_Type);
	CreateSplines(m_RootTree);
}

TArray<USplineComponent*>& ALSystemFoliage::GetSplineComponentArray()
{
	return m_SplineComponents;
}


// Called when the game starts or when spawned
void ALSystemFoliage::BeginPlay()
{
	Super::BeginPlay();
  
}

// Called every frame
void ALSystemFoliage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALSystemFoliage::PostActorCreated()
{
		
}

void ALSystemFoliage::OnConstruction(const FTransform& Transform)
{
	//(ELSystemType::PLANT,3);
	

	//CreateFoliageTypeInstance();
}

void ALSystemFoliage::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UE_LOG(LogTemp,Log, TEXT("Property changed: %s"), *PropertyChangedEvent.GetPropertyName().ToString())
	if(PropertyChangedEvent.GetPropertyName() == "M_Generation")
	{
		ResimulateTree();
	}
}


void ALSystemFoliage::PostEditMove(bool bFinished)
{

}

void ALSystemFoliage::ResimulateTree()
{
	for(auto i = 0; i < m_SplineComponents.Num();++i)
	{
		m_SplineComponents[i]->UnregisterComponent();
		m_SplineComponents[i]->DestroyComponent();
	}
	m_SplineComponents.Empty();

	for(auto i = 0; i < m_SplineMeshComponents.Num();++i)
	{
		m_SplineMeshComponents[i]->UnregisterComponent();
		m_SplineMeshComponents[i]->DestroyComponent();
	}
	m_SplineMeshComponents.Empty();

	for(auto i = 0; i < m_RootTree->GetBranches().Num(); ++i)
	{
		m_RootTree->GetBranches()[i]->ConditionalBeginDestroy();
	}


	m_LString = ULSystemGenerator::GenerateLString(m_Type, m_Generation);
	m_RootTree = ULSystemTurtle::IterateTurtle(m_LString,Root,m_Type);	
	CreateSplines(m_RootTree);
}



void ALSystemFoliage::CreateSplines(UTree* tree)
{
	for(auto i = 0; i < tree->GetBranches().Num(); i++)
	{
		auto branch = tree->GetBranches()[i];
		if(branch->ShouldDraw())
		{			
			auto newSpline  = NewObject<USplineComponent>(this);
			auto points = branch->GetPoints();
			newSpline->ClearSplinePoints();
			for(auto i = 0; i < points.Num(); i++)
			{
				newSpline->AddSplineWorldPoint(points[i]);
			}
			newSpline->RegisterComponentWithWorld(GetWorld());
			newSpline->SetUnselectedSplineSegmentColor(FLinearColor::Green);
			AddOwnedComponent(newSpline);
			m_SplineComponents.Add(newSpline);
			newSpline->AttachToComponent(Root, FAttachmentTransformRules::SnapToTargetIncludingScale);
			UE_LOG(LogTemp,Log,TEXT("ALSystemFoliage::CreateSplineMeshComponent: %s"), *branch->GetLString())
			CreateSplineMeshComponents(newSpline, branch->GetWidth());
			CreateLeafMeshes(branch, newSpline);
		}
	}

}

void ALSystemFoliage::CreateFoliageTypeInstance()
{
	//TActorIterator<AInstancedFoliageActor> foliageIterator(GetWorld());
	//AInstancedFoliageActor* foliageActor = *foliageIterator;

	//TArray<UInstancedStaticMeshComponent*> components;
 //   foliageActor->GetComponents<UInstancedStaticMeshComponent>(components);
 //   //UInstancedStaticMeshComponent* meshComponent = components[0];


	/*UInstancedStaticMeshComponent* meshComponent = NewObject<UInstancedStaticMeshComponent>(Root, UInstancedStaticMeshComponent::StaticClass(), NAME_None, RF_Transactional);
     meshComponent->AttachTo(Root);
     meshComponent->SetStaticMesh(Mesh->GetStaticMesh());
     meshComponent->RegisterComponent();*/


	/*FTransform transform = FTransform();
     for (int32 x = 1; x < 20; x++)
     {
         for (int32 y = 1; y < 20; y++)
         {
             transform.SetLocation(FVector(1000.f * x, 1000.f * y, 0.f));
             meshComponent->AddInstance(transform);
         }
     }*/

}

void ALSystemFoliage::CreateSplineMeshComponents(USplineComponent* spline, float width)
{
	if(spline->GetNumberOfSplinePoints() > 1){

			for(auto i = 0; i < spline->GetNumberOfSplinePoints() - 1; ++i)
			{
				auto startPos =spline->GetLocationAtSplinePoint(i,ESplineCoordinateSpace::Local);
				auto startTans =spline->GetTangentAtSplinePoint(i,ESplineCoordinateSpace::Local);
				auto endPos =spline->GetLocationAtSplinePoint(i+1,ESplineCoordinateSpace::Local);
				auto endTan =spline->GetTangentAtSplinePoint(i+1,ESplineCoordinateSpace::Local);

				if(startTans.X  == 0.f)
					startTans.X = 0.2f;
				
				auto newSplineMesh = NewObject<USplineMeshComponent>(this);
				
				newSplineMesh->SetStaticMesh(m_SplineMesh);
				newSplineMesh->SetMobility(EComponentMobility::Movable);
				newSplineMesh->SetStartAndEnd(startPos,startTans,endPos,endTan,true);
				newSplineMesh->SetStartScale(FVector2D(width,width));
				newSplineMesh->SetEndScale(FVector2D(width,width));
				m_SplineMeshComponents.Add(newSplineMesh);
				newSplineMesh->RegisterComponentWithWorld(GetWorld());
				AddOwnedComponent(newSplineMesh);
				newSplineMesh->AttachToComponent(Root, FAttachmentTransformRules::SnapToTargetIncludingScale);				
			}
		}
}

void ALSystemFoliage::CreateLeafMeshes(UTree* tree, USplineComponent* spline)
{

	UInstancedStaticMeshComponent *ISMComp = NewObject<UInstancedStaticMeshComponent>(this);
	ISMComp->RegisterComponent();
	ISMComp->SetStaticMesh(m_LeafMesh);
	ISMComp->SetFlags(RF_Transactional);
	this->AddInstanceComponent(ISMComp);



	for(auto i = 0; i < tree->GetLeaves().Num();++i)
	{
		auto newtran =  new FTransform();
		newtran->SetLocation(tree->GetLeaves()[i]->location);	
		newtran->SetRotation(tree->GetLeaves()[i]->orientation.ToOrientationQuat());
	}


}

void ALSystemFoliage::AgeFoliage(int newAge)
{
	m_Generation = newAge;
	ResimulateTree();
}


void ALSystemFoliage::ConvertMeshes()
{
	FVector ori, maxExt;
	this->GetActorBounds(false, ori, maxExt);
	
	
}

float ALSystemFoliage::GetScaleForAge(const float Age) const
{
	const FRichCurve* Curve = ScaleCurve.GetRichCurveConst();
	const float Time = FMath::Clamp(MaxAge == 0 ? 1.f : Age / MaxAge, 0.f, 1.f);
	const float Scale = Curve->Eval(Time);
	return ProceduralScale.Min + ProceduralScale.Size() * Scale;
}

float ALSystemFoliage::GetInitAge(FRandomStream & RandomStream) const
{
	return RandomStream.FRandRange(0, MaxInitialAge);
}

float ALSystemFoliage::GetNextAge(const float CurrentAge, const int32 NumSteps) const
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

bool ALSystemFoliage::GetSpawnsInShade() const
{
	return bCanGrowInShade && bSpawnsInShade;
}

ALSystemFoliage * ALSystemFoliage::Domination(ALSystemFoliage * A, ALSystemFoliage * B, ESimulationOverlap::Type OverlapType)
{
	return nullptr;
}

FBox ALSystemFoliage::GetBounds() const
{
	FVector ori, extent;
	this->GetActorBounds(false,ori,extent);
	return this->GetComponentsBoundingBox(false);
	//return FBox(ori - extent/2, ori + extent/2);
}









