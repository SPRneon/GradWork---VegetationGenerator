// Fill out your copyright notice in the Description page of Project Settings.

#include "LSystemInterpreter.h"


FString ULSystemInterpreter::m_LString = "";
FTransform ULSystemInterpreter::m_Transform;
TArray<FVector> ULSystemInterpreter::m_vLocations;

int ULSystemInterpreter::m_BranchDepth = 0;

TArray<FVector> ULSystemInterpreter::InterpretLString(FString lString,  FTransform transform)
{
	m_Transform = transform;
	m_LString = lString;	
	Iterate();

	return m_vLocations;
}

void ULSystemInterpreter::Iterate()
{
	Turtle turtle(m_Transform.GetRotation().GetUpVector(), 10.f, m_Transform.GetLocation());

	
	m_vLocations = turtle.Iterate(m_LString.GetCharArray());
}

