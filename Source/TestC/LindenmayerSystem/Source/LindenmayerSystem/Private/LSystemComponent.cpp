// Fill out your copyright notice in the Description page of Project Settings.

#include "LSystemComponent.h"
#include "Async/Future.h"
#include "Async/Async.h"
#include "GameFramework/Volume.h"
#include "Components/BrushComponent.h"
#include "Engine/LevelBounds.h"
#include "Misc/FeedbackContext.h"

#define LOCTEXT_NAMESPACE "Lindenmayer"

ULSystemComponent::ULSystemComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	TileOverlap = 0.f;
	ProceduralGuid = FGuid::NewGuid();
#if WITH_EDITORONLY_DATA
	bAllowLandscape = true;
	bAllowBSP = true;
	bAllowStaticMesh = true;
	bAllowTranslucent = false;
	bAllowFoliage = false;
	bShowDebugTiles = false;
#endif
}

FBox ULSystemComponent::GetBounds() const
{
	UBrushComponent* Brush = SpawningVolume ? SpawningVolume->GetBrushComponent() : nullptr;
	if (Brush)
	{
		return Brush->Bounds.GetBox();
	}
	else
	{
		AActor* LocalOwner = GetOwner();
		ULevel* Level = LocalOwner ? LocalOwner->GetLevel() : nullptr;
		ALevelBounds* LevelBoundsActor = Level ? Level->LevelBoundsActor.Get() : nullptr;
		if (LevelBoundsActor)
		{
			return LevelBoundsActor->GetComponentsBoundingBox(false);
		}
	}

	return FBox(ForceInitToZero);
}


void ULSystemComponent::PostEditImport()
{
	// The Guid should always be unique
	ProceduralGuid = FGuid::NewGuid();
}

#undef LOCTEXT_NAMESPACE