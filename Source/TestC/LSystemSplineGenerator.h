// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LSystemSplineGenerator.generated.h"

class USplineComponent;
class UTree;
class USplineMeshComponent;
class ALSystemFoliage;


/**
 * 
 */
UCLASS()
class TESTC_API ULSystemSplineGenerator : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
		UFUNCTION(BlueprintCallable, Category = "Lindenmayer") static void GenerateLSysSplines(ALSystemFoliage* owner , UTree* tree );
	
private:
	static void CreateSplines(UTree* tree);

	static ALSystemFoliage * m_Owner;
	static FTransform m_Transform;
	static USplineComponent* m_SplineComponent;
	static UTree* m_Tree;
	static USplineMeshComponent m_SplineMeshComponent;
	
};
