// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TestCGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class TESTC_API ATestCGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	void OnConstruction(const FTransform& Transform) override;
	
	
	
};
