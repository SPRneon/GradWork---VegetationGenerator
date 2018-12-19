// Fill out your copyright notice in the Description page of Project Settings.

#include "LSystemVolume.h"
#include "Components/BrushComponent.h"
#include "LSystemFoliageSpawner.h"



//ALSystemVolume::ALSystemVolume(const FObjectInitializer& ObjectInitializer)
//: Super(ObjectInitializer)
//{
//	ALSystemFoliageSpawner = ObjectInitializer.CreateDefaultSubobject<ALSystemFoliageSpawner>(this, TEXT("ProceduralFoliageComponent"));
//	ALSystemFoliageSpawner->SetSpawningVolume(this);
//
//	if (UBrushComponent* MyBrushComponent = GetBrushComponent())
//	{
//		MyBrushComponent->SetCollisionObjectType(ECC_WorldStatic);
//		MyBrushComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
//
//		// This is important because the volume overlaps with all procedural foliage
//		// That means during streaming we'll get a huge hitch for UpdateOverlaps
//		MyBrushComponent->bGenerateOverlapEvents = false;
//	}
//}
