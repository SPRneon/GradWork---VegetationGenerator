// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LSystemInterpreter.generated.h"

/**
 * 
 */
UCLASS()
class TESTC_API ULSystemInterpreter : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
		UFUNCTION(BlueprintCallable, Category = "Lindenmayer") static TArray<FVector> InterpretLString(FString lString,  FTransform transform);
	
	
	//Helpers
	class Turtle
	{

		struct TurtleState
		{
			FVector location;
			FVector orientation;			
		};
		FFloat16 m_Growth;
		TArray<TurtleState> m_States;
		
	public:
		Turtle(FVector orientation, FFloat16 growth, FVector location){	m_States.Add(TurtleState{ location,orientation });	m_Growth = growth;	}
		TArray<FVector> Iterate(TArray<TCHAR> commands)
		{
			TArray<FVector> locations;
			
			for(int i =0; i < commands.Num(); i++)
			{
				
				TurtleState curr = m_States.Last();				
					switch (commands[i])
					{
					case 'F':
					{
						curr.location += (m_Growth * curr.orientation);
						
						locations.Add(curr.location);
						break;
					}
					case '+':
					{
						curr.orientation.X += cosf(45);
						curr.orientation.Y += sinf(45);
						//curr.orientation.RotateAngleAxis(45, FVector(0, 0, 1));
						

						break;
					}
					case '-':
					{
						curr.orientation.X -= cosf(45);
						curr.orientation.Y -= sinf(45);
						//curr.orientation.RotateAngleAxis(-45, FVector(0, 0, 1));
						break;
					}
					case '[':
					{
						m_States.Add(TurtleState{ curr.location, curr.orientation });
						break;
					}
					case ']':
					{
						m_States.Pop();
						curr = m_States.Last();
						break;
					}
					case '0':
					{
						break;
					}
					default:
						break;
					}
				m_States.Last() = curr;
			}
			return locations;
		}		
	};

private:
	static FString m_LString;
	static FTransform m_Transform;
	static TArray<FVector> m_vLocations;
	static int m_BranchDepth;

	static void Iterate();
	static void Grow();
	

};
