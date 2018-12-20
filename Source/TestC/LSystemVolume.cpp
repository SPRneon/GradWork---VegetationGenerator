// Fill out your copyright notice in the Description page of Project Settings.

#include "LSystemVolume.h"
#include "Components/BrushComponent.h"

#include "LSystemComponent.h"
#include "LSystemFoliageSpawner.h"



ALSystemVolume::ALSystemVolume(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	LSystemComponent = ObjectInitializer.CreateDefaultSubobject<ULSystemComponent>(this, TEXT("LSystemComponent"));
	LSystemComponent->SetSpawningVolume(this);

	if (UBrushComponent* MyBrushComponent = GetBrushComponent())
	{
		MyBrushComponent->SetCollisionObjectType(ECC_WorldStatic);
		MyBrushComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

		// This is important because the volume overlaps with all procedural foliage
		// That means during streaming we'll get a huge hitch for UpdateOverlaps
		MyBrushComponent->bGenerateOverlapEvents = false;
	}
}

#if WITH_EDITOR

void ALSystemVolume::PostEditImport()
{
	// Make sure that this is the component's spawning volume
	LSystemComponent->SetSpawningVolume(this);
}

bool ALSystemVolume::GetReferencedContentObjects(TArray<UObject*>& Objects) const
{
	Super::GetReferencedContentObjects(Objects);

	if (LSystemComponent && LSystemComponent->LSystemSpawner)
	{
		Objects.Add(LSystemComponent->LSystemSpawner);
	}
	return true;
}

#endif