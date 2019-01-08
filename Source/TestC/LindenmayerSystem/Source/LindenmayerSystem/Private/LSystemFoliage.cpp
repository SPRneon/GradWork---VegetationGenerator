// Fill out your copyright notice in the Description page of Project Settings.

#include "LSystemFoliage.h"
#include "LSystemGenerator.h"
//#include "LSystemTree.h"
#include "LSytemTurtle.h"
#include "LSystemTree.h"
#include "Stats2.h"
#include "InstancedLSystemFoliage.h"
#include "LSystemFoliageInstance.h"
#include "LSystemSplineGenerator.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
#include "Components/BrushComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Runtime/Foliage/Public/InstancedFoliageActor.h"

#include "EngineUtils.h"
#include "ConstructorHelpers.h"
#include "LindenmayerSystem/Source/LindenmayerSystem/Public/LindemayerFoliageType.h"





// Sets default values
ALSystemFoliage::ALSystemFoliage()
{
	m_Transform = GetTransform();

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	const ConstructorHelpers::FObjectFinder<UStaticMesh> splineMesh(TEXT("/Game/VegetationGenerator/BranchSplineSmall.BranchSplineSmall"));
	m_SplineMesh = splineMesh.Object;
	

	
	const ConstructorHelpers::FObjectFinder<UStaticMesh> LeafMeshObj(TEXT("/Game/VegetationGenerator/Branches/Meshes/BranchLoaf.BranchLoaf"));
	m_LeafMesh = LeafMeshObj.Object;	
	m_IsAlive = true;

	m_LeafMeshComponents = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("LeafInstanceMesh"));
	

	
	
}

void ALSystemFoliage::Initialize(int age, ELSystemType type)
{

	m_LeafMeshComponents->SetStaticMesh(m_LeafMesh);
	m_LeafMeshComponents->SetFlags(RF_Transactional);
	this->AddInstanceComponent(m_LeafMeshComponents);


	m_Generation = age;
	m_Type = type;
	m_LString = ULSystemGenerator::GenerateLString(m_Type, m_Generation);	
	m_RootTree = ULSystemTurtle::IterateTurtle(m_LString,Root,m_Type);
	CreateSplines(m_RootTree);
}

void ALSystemFoliage::Initialize(const FLSysPotentialInstance& Inst)
{
	m_LeafMeshComponents = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("LeafInstanceMesh"));
	m_LeafMeshComponents->RegisterComponent();
	m_LeafMeshComponents->SetStaticMesh(m_LeafMesh);
	m_LeafMeshComponents->SetFlags(RF_Transactional);
	this->AddInstanceComponent(m_LeafMeshComponents);

	{
		m_Generation = Inst.DesiredInstance.Age;
		Type = Inst.DesiredInstance.FoliageType;
		m_Type= Type->LSystemType;
		m_LString = ULSystemGenerator::GenerateLString(m_Type, m_Generation);	
		m_RootTree = ULSystemTurtle::IterateTurtle(m_LString,Root,m_Type);
		CreateSplines(m_RootTree);
	}
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
		Initialize(m_Generation,m_Type);
		ResimulateTree();
	}
}


void ALSystemFoliage::PostEditMove(bool bFinished)
{
	UE_LOG(LogTemp,Log, TEXT("Actor (%s) was moved"), *this->GetName());
	if(bFinished)
	{
		Initialize(m_Generation,m_Type);
		ResimulateTree();
	}
}

void ALSystemFoliage::ResimulateTree()
{
	m_LeafMeshComponents->ClearInstances();

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
			newSpline->bShouldVisualizeScale = true;
			auto points = branch->GetPoints();
			newSpline->ClearSplinePoints();
			for(auto i = 0; i < points.Num(); i++)
			{
				newSpline->AddSplineWorldPoint(points[i]);
				newSpline->SetSplinePointType(i,ESplinePointType::Curve,false);
			}
			newSpline->RegisterComponentWithWorld(GetWorld());
			newSpline->SetUnselectedSplineSegmentColor(FLinearColor::Green);
			newSpline->UpdateSpline();
			AddOwnedComponent(newSpline);
			m_SplineComponents.Add(newSpline);
			newSpline->AttachToComponent(Root, FAttachmentTransformRules::SnapToTargetIncludingScale);
			CreateSplineMeshComponents(newSpline, branch);
			CreateLeafMeshes(branch, newSpline);
			if(branch->GetBranches().Num() < 1)
			{
				CreateEndLeaves(branch, newSpline);
			}
			
		}
	}

}



void ALSystemFoliage::CreateEndLeaves(UTree* tree, USplineComponent* spline)
{
	FTransform newtran;
	newtran = GetActorTransform();
	for(int i = 0; i < 3;++i)
	{
		FQuat dir =  spline->GetQuaternionAtTime(0.2f * i, ESplineCoordinateSpace::World);
		FVector pos = spline->GetLocationAtTime(0.2f * i,ESplineCoordinateSpace::World);

		newtran.SetLocation(pos);
		newtran.SetRotation(dir);
		m_LeafMeshComponents->AddInstanceWorldSpace(newtran);
	}
}


void ALSystemFoliage::CreateFoliageTypeInstance()
{

}

void ALSystemFoliage::CreateSplineMeshComponents(USplineComponent* spline, UTree* branch)
{
	

	if(spline->GetNumberOfSplinePoints() > 1){
		UE_LOG(LogTemp,Log,TEXT("ALSystemFoliage::CreateSplineMeshComponent: %s"), *branch->GetLString())

		float startWidth =  branch->GetWidth();
		float endWidth = branch->GetWidth();
		float deltaWidth = (startWidth - endWidth) / (spline->GetNumberOfSplinePoints() - 1); 
			for(auto i = 0; i < spline->GetNumberOfSplinePoints() - 1; ++i)
			{
				auto startPos =spline->GetLocationAtSplinePoint(i,ESplineCoordinateSpace::Local);
				auto startTans =spline->GetTangentAtSplinePoint(i,ESplineCoordinateSpace::Local);
				auto endPos =spline->GetLocationAtSplinePoint(i+1,ESplineCoordinateSpace::Local);
				auto endTan =spline->GetTangentAtSplinePoint(i+1,ESplineCoordinateSpace::Local);
				
				auto newSplineMesh = NewObject<USplineMeshComponent>(this);

				if(startTans == endTan)
				{
					startTans.X += 0.01f;
					startTans.Y += 0.01f;
					startTans.Z += 0.01f;
				}

				

				
				newSplineMesh->SetStaticMesh(m_SplineMesh);
				newSplineMesh->SetMobility(EComponentMobility::Movable);
				newSplineMesh->SetStartAndEnd(startPos,startTans,endPos,endTan,true);
				newSplineMesh->SetStartScale(FVector2D(startWidth - deltaWidth * (i-1) , startWidth- deltaWidth * (i-1)));
				newSplineMesh->SetEndScale(FVector2D(startWidth - deltaWidth * (i), startWidth - deltaWidth * (i)));
				m_SplineMeshComponents.Add(newSplineMesh);
				newSplineMesh->RegisterComponentWithWorld(GetWorld());
				AddOwnedComponent(newSplineMesh);
				newSplineMesh->AttachToComponent(Root, FAttachmentTransformRules::SnapToTargetIncludingScale);				
			}
		}
}

void ALSystemFoliage::CreateLeafMeshes(UTree* tree, USplineComponent* spline)
{
	FTransform newtran;
	newtran = GetActorTransform();
	for(auto i = 0; i < tree->GetLeaves().Num();++i)
	{
		//auto ori = tree->GetLeaves()[i]->orientation.UpVector;
		auto ori = spline->GetDirectionAtSplinePoint(spline->GetNumberOfSplinePoints() - 1,ESplineCoordinateSpace::Local);
		ori = ori.RotateAngleAxis(20.f,ori.RightVector);
		auto loc = tree->GetLeaves()[i]->location;
		newtran.SetLocation(newtran.GetLocation()+loc);
		newtran.SetRotation(newtran.GetRotation().RotateVector(ori).ToOrientationQuat());		
		m_LeafMeshComponents->AddInstanceWorldSpace(newtran);
		ori = ori.RotateAngleAxis(180,FVector(0,0,1));
		newtran.SetRotation(newtran.GetRotation().RotateVector(ori).ToOrientationQuat());
		//m_LeafMeshComponents->AddInstanceWorldSpace(newtran);
		
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


bool ALSystemFoliage::FoliageTrace(const UWorld* InWorld, FHitResult & OutHit, const FDesiredLSysInstance & DesiredInstance, FName InTraceTag, bool InbReturnFaceIndex, const FLSysTraceFilterFunc & FilterFunc)
{
	

	FCollisionQueryParams QueryParams(InTraceTag, SCENE_QUERY_STAT_ONLY(LSA_FoliageTrace), true);
	QueryParams.bReturnFaceIndex = InbReturnFaceIndex;


	const FVector Dir = (DesiredInstance.EndTrace - DesiredInstance.StartTrace).GetSafeNormal();
	const FVector StartTrace = DesiredInstance.StartTrace - (Dir * DesiredInstance.TraceRadius);

	TArray<FHitResult> Hits;
	FCollisionShape SphereShape;
	SphereShape.SetSphere(DesiredInstance.TraceRadius);
	InWorld->SweepMultiByObjectType(Hits, StartTrace, DesiredInstance.EndTrace, FQuat::Identity, FCollisionObjectQueryParams(ECC_WorldStatic), SphereShape, QueryParams);

	for (const FHitResult& Hit : Hits)
	{
		const AActor* HitActor = Hit.GetActor();

		//Check for blocking volume if present
		//TODO: Easily just make blocking volume inherit from lsystemvolume
		//if (DesiredInstance.PlacementMode == EFoliagePlacementMode::Procedural)
		//{
		//	if (const ALSystemBlockingVolume* ProceduralFoliageBlockingVolume = Cast<AProceduralFoliageBlockingVolume>(HitActor))
		//	{
		//		const AProceduralFoliageVolume* ProceduralFoliageVolume = ProceduralFoliageBlockingVolume->ProceduralFoliageVolume;
		//		if (ProceduralFoliageVolume == nullptr || ProceduralFoliageVolume->ProceduralComponent == nullptr || ProceduralFoliageVolume->ProceduralComponent->GetProceduralGuid() == DesiredInstance.ProceduralGuid)
		//		{
		//			return false;
		//		}
		//	}
		//	else if (HitActor && HitActor->IsA<AProceduralFoliageVolume>()) //we never want to collide with our spawning volume
		//	{
		//		continue;
		//	}
		//}

		const UPrimitiveComponent* HitComponent = Hit.GetComponent();
		check(HitComponent);

		// In the editor traces can hit "No Collision" type actors, so ugh. (ignore these)
		if (!HitComponent->IsQueryCollisionEnabled() || HitComponent->GetCollisionResponseToChannel(ECC_WorldStatic) != ECR_Block)
		{
			continue;
		}

		// Don't place foliage on invisible walls / triggers / volumes
		if (HitComponent->IsA<UBrushComponent>())
		{
			continue;
		}

		// Don't place foliage on itself
		/*if (const AInstancedFoliageActor* FoliageActor = Cast<AInstancedFoliageActor>(HitActor))
		{
			if (const FFoliageMeshInfo* FoundMeshInfo = FoliageActor->FindMesh(DesiredInstance.FoliageType))
			{
				if (FoundMeshInfo->Component == HitComponent)
				{
					continue;
				}
			}
		}*/

		if (FilterFunc && FilterFunc(HitComponent) == false)
		{
			// supplied filter does not like this component, so keep iterating
			continue;
		}

		bool bInsideProceduralVolumeOrArentUsingOne = true;
		if (DesiredInstance.PlacementMode == EFoliagePlacementMode::Procedural && DesiredInstance.ProceduralVolumeBodyInstance)
		{
			// We have a procedural volume, so lets make sure we are inside it.
			bInsideProceduralVolumeOrArentUsingOne = DesiredInstance.ProceduralVolumeBodyInstance->OverlapTest(Hit.ImpactPoint, FQuat::Identity, FCollisionShape::MakeSphere(1.f));	//make sphere of 1cm radius to test if we're in the procedural volume
		}

		OutHit = Hit;

		return bInsideProceduralVolumeOrArentUsingOne;

	}

	return false;
}

bool ALSystemFoliage::CheckCollisionWithWorld(const UWorld * InWorld, const ULSystemFoliageType * Settings, const FLSysFoliageInstance & Inst, const FVector & HitNormal, const FVector & HitLocation, UPrimitiveComponent * HitComponent)
{
	if (!Settings->CollisionWithWorld)
	{
		return true;
	}

	FTransform OriginalTransform = Inst.GetInstanceWorldTransform();
	OriginalTransform.SetRotation(FQuat::Identity);

	FMatrix InstTransformNoRotation = OriginalTransform.ToMatrixWithScale();
	OriginalTransform = Inst.GetInstanceWorldTransform();






	return false;
}

ALSystemFoliage * ALSystemFoliage::Domination(ALSystemFoliage * A, ALSystemFoliage * B, ESimulationOverlap::Type OverlapType)
{
	//TODO
	return nullptr;
}

FBox ALSystemFoliage::GetBounds() const
{
	FVector extent;
	FVector center;
	for(auto spline: m_SplineComponents)
	{
		auto box = spline->Bounds.GetBox();
		center += box.GetCenter();
		if(extent.Z < box.GetExtent().Z)
			extent.Z = box.GetExtent().Z;
		if(extent.X < box.GetExtent().X)
			extent.X = box.GetExtent().X;
		if(extent.Y < box.GetExtent().Y)
			extent.Y = box.GetExtent().Y;
	}
	center /= m_SplineComponents.Num();

	return FBox{center - extent, center + extent};
	
}









