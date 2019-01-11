// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Helpers.h"
#include "LSytemTurtle.generated.h"





class UTree;
/**
 * 
 */
UCLASS()
class ULSystemTurtle : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
		UFUNCTION(BlueprintCallable, Category = "Lindenmayer") static UTree* IterateTurtle(FString lString, USceneComponent* root, ELSystemType type = ELSystemType::PLANT);
private:



	//Func
	static UTree* StartTurtle();
	static bool SetVar(ELSystemType type);

	//Rot Matrices
	static FRotator m_Rotator;

	//Data
	static UTree* m_Tree;
	static USceneComponent* m_Owner;
	static FTransform m_Tranform;
	static FString m_LString;
	static FVector2D m_Angle;
	static float m_Growth;
	static float m_Width;
};
