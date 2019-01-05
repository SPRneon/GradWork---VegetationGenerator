// Fill out your copyright notice in the Description page of Project Settings.
#include "LSystemGenerator.h"
#include "Helpers.h"
//#include "TestC.h"



FString ULSystemGenerator::m_State = "";
std::vector<ULSystemGenerator::LSystemRule*> ULSystemGenerator::m_vRules;

FString ULSystemGenerator::GenerateLString(ELSystemType Type, int gen)
{
	m_State = "";
	
	
	switch (Type)
	{
	case ELSystemType::PLANT:
	{
		UE_LOG(LogTemp, Log, TEXT("This is a plant"));
		SetState("F");
		AddWeightedRule('F', "F[//F][&&F]",0.10f);
		AddRule('F', "F++");
		AddRule('F', "FF");
		AddRule('F', "|F");
		AddWeightedRule('F', "F[FL]", 2.0f);
		break;
	}
	case ELSystemType::WEED:
	{
		UE_LOG(LogTemp, Log, TEXT("This is a weed"));
		SetState("F");
		AddRule('F', "F[+F]F[-F]F");
		AddRule('F', "F[+F][-F]");
		AddRule('F', "F[\\\\F//F][&&F^^F][//F\\\\F][^^F&&F]");
		break;
	}
	case ELSystemType::BUSH:
	{
		UE_LOG(LogTemp, Log, TEXT("This is a bush"));
		SetState("X");
		AddRule('X', "F[+X]F[-X]+X");
		AddRule('F', "FF");
		break;
	}
	case ELSystemType::TREE:
	{
		UE_LOG(LogTemp, Log, TEXT("This is a tree"));
		SetState("X");
		AddRule('X', "F-[[X]+X]+F[+FX]-X");
		AddRule('F', "FF");
		break;
	}
	case ELSystemType::FLOWER:
	{
		UE_LOG(LogTemp, Log, TEXT("This is a flower"));
		SetState("X");
		AddRule('X', "I+[X+L]--//[--E]I[++E]-[XL]++XL");
		AddRule('I', "FS[//&&E][//^^E]FS");
		AddRule('S', "SFS");
		AddRule('E', "+F-FF-F+");
		AddRule('E', "+F-FF-F");
		AddRule('L', "[&&&P]");
	}
	default:
		break;
	}
	m_State.AppendChar('\0');

	for(int i = 0; i < gen; i++)
		Iterate();

	
	return m_State;
}




void ULSystemGenerator::AddRule(TCHAR head, FString tail, TCHAR pre, TCHAR post)
{	
	m_vRules.push_back(new LSystemRule(head, tail, pre, post));
}

void ULSystemGenerator::AddWeightedRule(TCHAR head, FString tail, float weight, TCHAR pre, TCHAR post)
{		
	m_vRules.push_back(new LSystemRule(head, tail, pre, post,weight));
}

void ULSystemGenerator::CheckRules(LSystemRule* newRule)
{
	
}

FString& ULSystemGenerator::Replace(TCHAR& head, TCHAR pre, TCHAR post)
{
	static FString seq;
	seq.Reset();

	TArray<LSystemRule*> matches;
	float totalWeight = 0.f;
	for (int i = 0; i < m_vRules.size(); i++)
	{
		if (m_vRules[i]->isMatch(head, pre, post))
		{
			totalWeight += m_vRules[i]->GetWeight();
			matches.Add(m_vRules[i]);
			//return seq.Append(m_vRules[i]->getTail());
		}
	}

	UE_LOG(LogTemp,Log,TEXT("Rule matches: %d"), matches.Num())

	float rnd = FMath::FRandRange(0, totalWeight);
	for (int i = 0; i < matches.Num(); i++)
	{		
		if (rnd < matches[i]->GetWeight())
			return matches[i]->getTail();
		rnd -= matches[i]->GetWeight();
	}
	seq.AppendChar(head);
	return seq;
}

void ULSystemGenerator::Iterate()
{
	FString newState;

	//UE_LOG(LogTemp,Log, TEXT("Iterate for string: %s"),*m_State);


	for (int i = 0; i < m_State.Len(); i++)
	{
		if(m_State.GetCharArray()[i] == '\0')
			break;

		TCHAR pre = '\0', head = '\0', post = '\0';
		if(i>0)
			pre = m_State.GetCharArray()[i-1];
		if(i < m_State.Len() - 1)
			post = m_State.GetCharArray()[i+1];
		head = m_State.GetCharArray()[i];
		
		FString &replacement = Replace(head,pre,post);
		
		newState.Append(replacement);
		

	}
	//UE_LOG(LogTemp,Log,TEXT("After replaceing: %s"), *newState);
	

	m_State = newState;
}