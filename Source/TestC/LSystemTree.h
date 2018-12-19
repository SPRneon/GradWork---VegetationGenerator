// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include <Components/SplineComponent.h>
#include "LSystemTree.generated.h"



UCLASS(BlueprintType)
class TESTC_API UTree : public UObject
{
public:
	GENERATED_BODY()
		UFUNCTION(BlueprintCallable, Category = "Lindenmayer")  TArray<FString> GetLStrings();
		UFUNCTION(BlueprintCallable, Category = "Lindenmayer") void BreakTree( FString& LString,  FVector& Position, FVector& Orientation, FVector& EndPos, FVector& EndOri, TArray<FVector>& Points, bool& hasGrown);
		UFUNCTION(BlueprintCallable, Category = "Lindenmayer") TArray<UTree*> GetBranches();
		UFUNCTION(BlueprintCallable,Category = "LindenMayer") TArray<FVector> GetPoints() const;
		UFUNCTION(BlueprintCallable,Category = "LindenMayer") bool ShouldDraw() const;
		UFUNCTION(BlueprintCallable, Category = "Lindenmayer")  FString GetLString() const;
		UFUNCTION(BlueprintCallable, Category = "Lindenmayer")  void SetLString(FString lstring);

public:
	UTree() {};
	UTree(FVector pos, FVector ori, FString lString = "") { m_StartPos = pos; m_StartOri = ori; m_LString = lString; }
	~UTree() { m_Branches.Empty(); }

	friend class ULSystemTurtle;




public:
	struct Leaf
	{
		FVector location;
		FVector orientation;
	};

	float GetWidth() const;
	TArray<Leaf*> GetLeaves();

private:
	//func
	void FetchLStrings(TArray<FString>& strings);
	void FetchBranches(TArray<UTree*>& branches);


	//Data
	int level = 0;
	FVector m_StartPos;
	FVector m_StartOri;
	FVector m_EndPos;
	FVector m_EndOri;
	FString m_LString;
	float m_Width;
	bool m_Draw = false;
	

	UTree* m_Root;
	TArray<UTree*> m_Branches;
	TArray<FVector> m_Points;
	TArray<Leaf*> m_Leaves;
};