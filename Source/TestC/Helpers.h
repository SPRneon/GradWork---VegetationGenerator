// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
/**
 * 
 */
UENUM(BlueprintType)
enum class ELSystemType : uint8
{
	PLANT UMETA(DisplayName = "Plant"),
	WEED UMETA(DisplayName = "Weed"),
	BUSH UMETA(DisplayName = "Bush"),
	TREE UMETA(DisplayName = "Tree"),
	FLOWER UMETA(DisplayName = "Flower"),
};


