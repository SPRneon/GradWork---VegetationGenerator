// Fill out your copyright notice in the Description page of Project Settings.

#include "LSytemTurtle.h"
#include "LSystemTree.h"
#include "Components/SplineComponent.h"

UTree* ULSystemTurtle::m_Tree = nullptr;

USceneComponent* ULSystemTurtle::m_Owner = nullptr;
FTransform ULSystemTurtle::m_Tranform;
FString ULSystemTurtle::m_LString = "";
FRotator ULSystemTurtle::m_Rotator;
FVector2D ULSystemTurtle::m_Angle;
float ULSystemTurtle::m_Growth;



UTree* ULSystemTurtle::IterateTurtle(FString lString, USceneComponent* root, ELSystemType type)
{
	m_LString = lString;
	
	m_Tranform = root->GetComponentTransform();
	auto rootPos = m_Tranform.GetLocation();
	auto rootOri = m_Tranform.GetRotation().GetUpVector();	

	m_Tree = NewObject<UTree>();
	m_Tree->m_StartPos = FVector::ZeroVector;
	m_Tree->m_StartOri = rootOri;
	

	//UE_LOG(LogTemp,Log,TEXT("StartOri: %s"), root)
	SetVar(type);
	StartTurtle();


	
	return m_Tree;
}

void ULSystemTurtle::SetVar(ELSystemType type)
{
	switch(type)
	{
	case ELSystemType::PLANT:
	{
		m_Angle.X = 45.f;
		m_Angle.Y = 22.5f;
		m_Growth = 10.f;
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
		break;
	}
}

void ULSystemTurtle::StartTurtle()
{
	int level = m_Tree->level;
	FVector currPos = m_Tree->m_StartPos;	
	FVector currOri = m_Tree->m_StartOri;

	FVector currUp = currOri.UpVector;
	FVector currFor = currOri.UpVector;
	FVector currRight = currOri.RightVector;

	
	FVector2D angleRad = m_Angle * (PI / 180);
	

	UTree* curr = m_Tree;

	
	for (int i = 0; i < m_LString.Len(); i++)
	{
		TCHAR c = m_LString.GetCharArray()[i];
		
		
		
		switch (c)
		{
		case 'F':
		{	
			

			curr->m_LString.AppendChar(c);
			currPos += (currOri * m_Growth);
			curr->m_Draw = true;	
			curr->m_Points.Add(currPos);		
			break;
		}
		case '+':
		{
			UE_LOG(LogTemp,Log, TEXT("went in + while ori: %s"), *currOri.ToString())
			curr->m_LString.AppendChar(c);
			currOri =  currOri.RotateAngleAxis(m_Angle.X, currUp);
			break;
		}
		case '-':
		{
			curr->m_LString.AppendChar(c);
			currOri = currOri.RotateAngleAxis(-m_Angle.X, currUp);
			break;
		}
		case '[':
		{
			level++;
			auto newTree = NewObject<UTree>();
			newTree->m_Root = curr;
			newTree->level = level;
			newTree->m_StartPos = currPos;
			newTree->m_Points.Add(currPos);
			newTree->m_StartOri = currOri;
			curr->m_EndOri = currOri;
			curr->m_Branches.Add(newTree);			
			curr = newTree;
			curr->m_LString.AppendChar(c);
			break;
		}
		case ']':
		{	
			curr->m_LString.AppendChar(c);
			--level;
			curr = curr->m_Root;
			currOri = curr->m_EndOri;
			currPos = curr->m_Points.Last();
			break;
		}
		case 'X':
		{			
			curr->m_LString.AppendChar(c);
			break;
		}
		case '&':
		{
			curr->m_LString.AppendChar(c);
			currOri = currOri.RotateAngleAxis(m_Angle.Y, currRight);

			break;
		}
		case '^':
		{
			curr->m_LString.AppendChar(c);
			currOri = currOri.RotateAngleAxis(-m_Angle.Y, currRight);

			break;
		}
		case '\\':
		{
			curr->m_LString.AppendChar(c);
			currOri = currOri.RotateAngleAxis(m_Angle.Y, currFor);

			break;
		}
		case '/':
		{
			curr->m_LString.AppendChar(c);
			currOri = currOri.RotateAngleAxis(-m_Angle.Y, currFor);

			break;
		}
		case '|':
		{
			currOri = currOri.RotateAngleAxis(180,currUp);
			curr->m_LString.AppendChar(c);
			break;
		}
		default:
			break;
		}
		
		curr->m_EndOri = currOri;
		curr->m_EndPos = currPos;

	}
	


	
}

