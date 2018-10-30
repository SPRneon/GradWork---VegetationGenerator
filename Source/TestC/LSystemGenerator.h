// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <vector>
#include <string>
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LSystemGenerator.generated.h"






/**
 * 
 */
UCLASS()
class TESTC_API ULSystemGenerator : public UBlueprintFunctionLibrary
{

	GENERATED_BODY()
public:
		UFUNCTION(BlueprintCallable, Category = "Lindenmayer") static FString GenerateLString(ELSystemType Type, int gen);
	


//Helpers
public:
	class LSystemRule
	{
		TCHAR m_Head;
		FString m_Tail;
		TCHAR m_Pre;
		TCHAR m_Post;
		float m_Chance = 1.0f;
	public:
		LSystemRule(TCHAR head, FString tail, TCHAR pre = '\0', TCHAR post = '\0', float chance = 1.0f) { m_Head = head; m_Tail = tail; m_Chance = chance; m_Pre = pre; m_Post = post;}
		 bool isMatch(TCHAR head, char pre = '\0', char post = '\0') const
		{	
			

			bool test = true;
			if(m_Pre != '\0' && m_Pre != pre)
				test = false;
			if(m_Post != '\0' && m_Post != post)
				test = false;
			if(m_Head != head)
				test = false;
			
			return test;
		}
			FString& getTail() { return m_Tail; }
			float GetWeight() const { return m_Chance; }
			
			
	};
private:
	//Data
	static FString m_State;
	static std::vector<LSystemRule*> m_vRules;
	static std::vector<TCHAR> m_vHeads;
	static std::vector<FString> m_vTails;

	//Func
	static void SetState(FString state) { m_State = state; }
	static FString& GetState() { return m_State; }
	static void AddRule(TCHAR head, FString tail, TCHAR pre = '\0', TCHAR post = '\0');
	static void CheckRules(LSystemRule* newRule);
	static FString& Replace(TCHAR &head, TCHAR pre = '\0', TCHAR post = '\0');
	static void Iterate();

	//Operator

	
};
