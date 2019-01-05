// Fill out your copyright notice in the Description page of Project Settings.

#include "LSytemTurtle.h"
#include "LSystemTree.h"
#include "Components/SplineComponent.h"

#include <string>

UTree* ULSystemTurtle::m_Tree = nullptr;

USceneComponent* ULSystemTurtle::m_Owner = nullptr;
FTransform ULSystemTurtle::m_Tranform;
FString ULSystemTurtle::m_LString = "";
FRotator ULSystemTurtle::m_Rotator;
FVector2D ULSystemTurtle::m_Angle;
float ULSystemTurtle::m_Growth;
float ULSystemTurtle::m_Width;

//General Log
DEFINE_LOG_CATEGORY(LogLindenmayer);

UTree* ULSystemTurtle::IterateTurtle(FString lString, USceneComponent* root, ELSystemType type)
{
	m_LString = lString;
	
	m_Tranform = root->GetComponentTransform();
	auto rootPos = m_Tranform.GetLocation();
	auto rootOri = m_Tranform.GetRotation().GetUpVector();	

	m_Tree = NewObject<UTree>();
	m_Tree->m_StartPos = FVector::ZeroVector;
	m_Tree->m_StartOri = rootOri;
	m_Tree->m_Draw = true;
	m_Tree->m_Branches.Empty();



	if(SetVar(type))
		return StartTurtle();
	else
	{
		UE_LOG(LogLindenmayer, Log, TEXT("Selected Type for %s has no variables set in the Turtle"), *root->GetName());
		return nullptr;
	}

}


bool ULSystemTurtle::SetVar(ELSystemType type)
{
	switch(type)
	{
	case ELSystemType::PLANT:
	{
		m_Angle.X = 45.f;
		m_Angle.Y = 22.5f;
		m_Growth = 100.f;
		m_Width = 5.0f;
		break;
	}
	case ELSystemType::WEED:
	{
		m_Angle.X = 45.f;
		m_Angle.Y = 22.5f;
		m_Growth = 10.f;
		break;
	}
	case ELSystemType::BUSH:
	{
		m_Angle.X = 45.f;
		m_Angle.Y = 22.5f;
		m_Growth = 10.f;
		break;
	}
	case ELSystemType::TREE:
	{
		m_Angle.X = 45.f;
		m_Angle.Y = 22.5f;
		m_Growth = 10.f;
		break;
	}
	case ELSystemType::FLOWER:
	{
		m_Angle.X = 45.f;
		m_Angle.Y = 22.5f;
		m_Growth = 10.f;
		break;
	}
	default:
		return false;
		break;
	}
	return true;
}

UTree* ULSystemTurtle::StartTurtle()
{
	int level = m_Tree->level;
	int deepLevel = level;
	FVector currPos = m_Tree->m_StartPos;
	m_Tree->m_Points.Add(currPos);
	FVector currOri = m_Tree->m_StartOri;
	m_Tree->m_Width = m_Width;

	FVector currUp = currOri.UpVector;
	FVector currFor = currOri.UpVector;
	FVector currRight = currOri.RightVector;

	
	FVector2D angleRad = m_Angle * (PI / 180);
	

	UTree* curr = m_Tree;

	
	for (int i = 0; i < m_LString.Len(); i++)
	{
		TCHAR c = m_LString.GetCharArray()[i];
		
		//currOri.Normalize();
		
		
		switch (c)
		{
			//GROWTH
		case 'F':
		{				
			curr->SetLString(curr->m_LString + c);
			currOri += (currOri - FVector(1,0,0)) / 3;
			
			currPos += (currOri.GetSafeNormal() * (m_Growth / (curr->level + 1)));
			curr->m_Draw = true;	
			curr->m_Points.Add(currPos);		
			break;
		}
			//BRANCHING
		case '[':
		{
			if(i != 0)
				level++;
			auto newTree = NewObject<UTree>();
			newTree->m_Root = curr;
			newTree->level = level;
			newTree->m_StartPos = currPos;
			newTree->m_Points.Add(currPos);
			newTree->m_StartOri = currOri;
			newTree->m_Width = FMath::Max(2.0f,  2/3 * curr->m_Width);
			curr->m_EndOri = currOri;
			curr->m_Branches.Add(newTree);			
			curr = newTree;
			curr->SetLString(curr->m_LString + c);
			break;
		}
		case ']':
		{	
			curr->SetLString(curr->m_LString + c);

			--level;
			if(level != -1)				
				curr = curr->m_Root;

			currOri = curr->m_EndOri;
			currPos = curr->m_Points.Last();
			break;
		}
			//ROTATION
			///Y AXIS
		case '+':
		{
			
			curr->SetLString(curr->m_LString + c);
			currOri =  currOri.RotateAngleAxis(m_Angle.X, currUp);
				
			break;
		}
		case '-':
		{
			curr->SetLString(curr->m_LString + c);
			currOri = currOri.RotateAngleAxis(-m_Angle.X, currUp);
			break;
		}
		case '|':
		{
			currOri = currOri.RotateAngleAxis(180,currUp);
			curr->SetLString(curr->m_LString + c);
			break;
		}
			///X-AXIS
		case '&':
		{
			curr->m_LString.AppendChar(c);
			currOri = currOri.RotateAngleAxis(m_Angle.Y, FVector(0,0,1));
			FMath::ClampAngle(currOri.Z,0.2f,360);

			break;
		}
		case '^':
		{
			curr->SetLString(curr->m_LString + c);
			currOri = currOri.RotateAngleAxis(-m_Angle.Y, FVector(0,0,1));
			FMath::ClampAngle(currOri.Z,0.2f,360);
			break;
		}
			///Z AXIS
		case '\\':
		{
			curr->SetLString(curr->m_LString + c);
			currOri = currOri.RotateAngleAxis(m_Angle.Y, FVector(0,1,0));
			FMath::ClampAngle(currOri.Y,0.2f,360);
			break;
		}
		case '/':
		{
			curr->SetLString(curr->m_LString + c);
			currOri = currOri.RotateAngleAxis(-m_Angle.Y, FVector(0,1,0));
			FMath::ClampAngle(currOri.Y,0.2f,360);
			break;
		}

			//SEGMENTS
		case 'X':
		{	
			curr->SetLString(curr->m_LString + c);
			break;
		}
		case 'L':
		{
			auto newLeaf = new UTree::Leaf();
			newLeaf->location = currPos;
			newLeaf->orientation =  currOri;
			curr->m_Leaves.Add(newLeaf);
			break;
			
		}

		default:
			break;
		}
		
		curr->m_EndOri = currOri;
		curr->m_EndPos = currPos;


		deepLevel = FMath::Max(level, deepLevel);
	}

	
	
	return curr;
}

