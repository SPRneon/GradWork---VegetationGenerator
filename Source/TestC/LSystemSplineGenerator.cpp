// Fill out your copyright notice in the Description page of Project Settings.

#include "LSystemSplineGenerator.h"
#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "LSystemTree.h"
#include "LSystemFoliage.h"
#include "Runtime/Engine/Classes/Components/SplineMeshComponent.h"
//#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"

ALSystemFoliage* ULSystemSplineGenerator::m_Owner = nullptr;
FTransform ULSystemSplineGenerator::m_Transform;
USplineComponent* ULSystemSplineGenerator::m_SplineComponent = nullptr;
UTree* ULSystemSplineGenerator::m_Tree = nullptr;
//USplineMeshComponent ULSystemSplineGenerator::m_SplineMeshComponent;

void ULSystemSplineGenerator::GenerateLSysSplines(ALSystemFoliage* owner, UTree* tree)
{
	m_Owner = owner;
	m_Transform = m_Owner->GetRootComponent()->GetComponentTransform();
	m_Tree = tree;	
	CreateSplines(m_Tree);


}

void ULSystemSplineGenerator::CreateSplines(UTree* tree)
{
	//UWhateverComponent* NewComponent = ConstructObject<UWhateverComponent>(UWhateverComponent::StaticClass(), this, TEXT("ComponentName"));
 //
 //NewComponent->RegisterComponent();
 //NewComponent->OnComponentCreated(); // Might need this line, might not.
 //NewComponent->AttachTo(GetRootComponent(), SocketName /* NAME_None */);
	
	

	if(tree->ShouldDraw())
	{
		auto newSpline = NewObject<USplineComponent>(m_Owner,TEXT("Spline"));
		//newSpline->RegisterComponent();
		//newSpline->OnComponentCreated();
		//newSpline->AttachTo(m_Owner->GetRootComponent());
		/*newSpline->ClearSplinePoints();
		auto points = tree->GetPoints();
		for(auto i = 0; i < points.Num();i++)
		{
			newSpline->AddSplineWorldPoint(points[i]);
		}*/
		m_Owner->GetSplineComponentArray().Add(newSpline);	
		
	}

	

	

	/*if(tree->ShouldDraw())
	{
		auto points = tree->GetSplinePoints();
		auto newSpline = NewObject<USplineComponent>();
		newSpline->SetLocationAtSplinePoint(0, points[0].Position,ESplineCoordinateSpace::World);
		newSpline->SetLocationAtSplinePoint(1, points[1].Position,ESplineCoordinateSpace::World);
		for(auto i = 2; i < points.Num();++i)
		{
			newSpline->AddPoint(points[i],false);			
		}
		newSpline->UpdateSpline();
		newSpline->AttachToComponent(m_Owner->GetRootComponent(),FAttachmentTransformRules::KeepWorldTransform);
	}*/
	

	//Recursive
	//if(tree->GetBranches().Num() > 0)
	//{
	//	for(auto branch : tree->GetBranches())
	//	{
	//		CreateSplines(branch);
	//	}
	//}
}

