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
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));	
	const ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObj(TEXT("/Engine/EngineMeshes/Cube.Cube"));
	Mesh->SetStaticMesh(MeshObj.Object);
	Mesh->SetWorldScale3D(FVector(0.25f,0.25f,0.25f));
	//Mesh->AttachToComponent(Root,FAttachmentTransformRules::KeepWorldTransform);
	//Mesh->AttachTo(Root);

	//auto newSpline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	//m_SplineComponents.Add(newSpline);
	//m_SplineComponents[0]->AttachTo(Root);
	
}

void ALSystemFoliage::Initialize(ELSystemType type, int gen)
{
	m_Type = type;
	m_Generation = gen;
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
	/*Initialize(ELSystemType::PLANT,3);
	m_LString = ULSystemGenerator::GenerateLString(m_Type, m_Generation);	
	m_RootTree = ULSystemTurtle::IterateTurtle(m_LString,Root,m_Type);	
	CreateSplines(m_RootTree);

	CreateFoliageTypeInstance();*/
	
	
}

void ALSystemFoliage::OnConstruction(const FTransform& Transform)
{
		Initialize(ELSystemType::PLANT,3);
	m_LString = ULSystemGenerator::GenerateLString(m_Type, m_Generation);	
	m_RootTree = ULSystemTurtle::IterateTurtle(m_LString,Root,m_Type);	
	CreateSplines(m_RootTree);

	CreateFoliageTypeInstance();
}

void ALSystemFoliage::CreateSplines(UTree* tree)
{
	

	//Drawing
	if(tree->ShouldDraw())
	{
		auto newSpline  = NewObject<USplineComponent>(this,TEXT("RootSpline"));
		auto points = tree->GetPoints();
		newSpline->ClearSplinePoints();
		for(auto i = 0; i < points.Num(); i++)
		{
			UE_LOG(LogTemp,Log, TEXT("Point position: %s"), *points[i].ToString())
			newSpline->AddSplineLocalPoint(points[i]);
		}
		newSpline->RegisterComponentWithWorld(GetWorld());
		AddOwnedComponent(newSpline);
		newSpline->AttachToComponent(Root, FAttachmentTransformRules::SnapToTargetIncludingScale);
		m_SplineComponents.Add(newSpline);

		if(newSpline->GetNumberOfSplinePoints() > 1){
			for(auto i = 0; i < newSpline->GetNumberOfSplinePoints() - 1; ++i)
			{
				auto startPos =newSpline->GetLocationAtSplinePoint(i,ESplineCoordinateSpace::World);
				auto startTans =newSpline->GetTangentAtSplinePoint(i,ESplineCoordinateSpace::World);
				auto endPos =newSpline->GetLocationAtSplinePoint(i+1,ESplineCoordinateSpace::World);
				auto endTan =newSpline->GetTangentAtSplinePoint(i+1,ESplineCoordinateSpace::World);

				auto newSplineMesh = NewObject<USplineMeshComponent>(this);
				newSplineMesh->SetStaticMesh(Mesh->GetStaticMesh());
				newSplineMesh->SetStartAndEnd(startPos,startTans,endPos,endTan,true);

				newSplineMesh->RegisterComponentWithWorld(GetWorld());
				AddOwnedComponent(newSplineMesh);
				newSplineMesh->AttachToComponent(Root, FAttachmentTransformRules::SnapToTargetIncludingScale);
				m_SplineMeshComponents.Add(newSplineMesh);
			}
		}


	}
	

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
			UE_LOG(LogTemp,Log, TEXT("Point position: %s"), *points[i].ToString())
			newSpline->AddSplineLocalPoint(points[i]);
		}
		newSpline->RegisterComponentWithWorld(GetWorld());
		newSpline->SetUnselectedSplineSegmentColor(FLinearColor::Green);
		AddOwnedComponent(newSpline);
		newSpline->AttachToComponent(Root, FAttachmentTransformRules::SnapToTargetIncludingScale);

		
		auto newSplineMesh = NewObject<USplineMeshComponent>(this);
		

		m_SplineComponents.Add(newSpline);
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


	UInstancedStaticMeshComponent* meshComponent = NewObject<UInstancedStaticMeshComponent>(Root, UInstancedStaticMeshComponent::StaticClass(), NAME_None, RF_Transactional);
     meshComponent->AttachTo(Root);
     meshComponent->SetStaticMesh(Mesh->GetStaticMesh());
     meshComponent->RegisterComponent();


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





