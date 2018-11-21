// Fill out your copyright notice in the Description page of Project Settings.

#include "LSystemTree.h"
#include "Components/SplineComponent.h"


TArray<FString> UTree::GetLStrings()
{
	TArray<FString> strings;

	
	
	FetchLStrings(strings);

	return strings;
}

TArray<UTree*> UTree::GetBranches()
{
	TArray<UTree*> branches;

	FetchBranches(branches);

	return branches;

}

TArray<FVector> UTree::GetPoints() const
{
	return m_Points;
}

bool UTree::ShouldDraw() const
{
	return m_Draw;
}

FString UTree::GetLString() const
{
	return m_LString;
}

void UTree::SetLString(FString lstring)
{
	m_LString = lstring;
}







void UTree::BreakTree( FString& LString,  FVector& Position, FVector& Orientation , FVector& EndPos, FVector& EndOri, TArray<FVector>& Points, bool& hasGrown)
{
	LString = m_LString;
	Position = m_StartPos;
	Orientation = m_StartOri;
	EndPos = m_EndPos;
	EndOri = m_EndOri;
	Points = m_Points;
	hasGrown = m_Draw;

}

void UTree::FetchLStrings(TArray<FString>& strings)
{
	strings.Add(this->m_LString);

	if (m_Branches.Num() > 0 )
	{		
		for (auto tree : m_Branches)
		{
			tree->FetchLStrings(strings);
		}
	}

}



void UTree::FetchBranches(TArray<UTree*>& branches)
{
	branches.Add(this);

	if (m_Branches.Num() > 0)
	{
		for (auto tree : m_Branches)
		{
			tree->FetchBranches(branches);
		}
	}

}