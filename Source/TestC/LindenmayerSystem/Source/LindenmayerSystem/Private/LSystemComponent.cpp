// Fill out your copyright notice in the Description page of Project Settings.

#include "LSystemComponent.h"
#include "LSystemFoliageSpawner.h"
#include "Engine/World.h"
#include "LSystemTile.h"
#include "Async/Future.h"
#include "Async/Async.h"
#include "GameFramework/Volume.h"
#include "Components/BrushComponent.h"
#include "Engine/LevelBounds.h"
#include "Misc/FeedbackContext.h"
#include "PropertyEditorModule.h"
#include "ModuleManager.h"
#include "LSystemVolume.h"
#include "LSystemComponentDetails.h"

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

		//Custom properties
     FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	 //PropertyModule.RegisterCustomPropertyTypeLayout("LSystemFoliageSpawner", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FLSystemComponentDetails::MakeInstance));
	 PropertyModule.RegisterCustomClassLayout("LSystemComponent", FOnGetDetailCustomizationInstance::CreateStatic(&FLSystemComponentDetails::MakeInstance));
	 UE_LOG(LogTemp,Log,TEXT("Went in the constructor"));
}

FBox2D GetTileRegion(const int32 X, const int32 Y, const float InnerSize, const float Overlap)
{
	// For tiles on the bottom and the left (that have no preceding neighbor to overlap them), have the
	// tile fill that space with its own content
	const float BottomLeftX = X == 0 ? -Overlap : Overlap;
	const float BottomLeftY = Y == 0 ? -Overlap : Overlap;
	FBox2D Region(FVector2D(BottomLeftX, BottomLeftY), FVector2D(InnerSize + Overlap, InnerSize + Overlap));

	return Region;
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

FBodyInstance * ULSystemComponent::GetBoundsBodyInstance() const
{
		UBrushComponent* Brush = SpawningVolume ? SpawningVolume->GetBrushComponent() : nullptr;
	if (Brush)
	{
		return Brush->GetBodyInstance();
	}

	return nullptr;
}

void ULSystemComponent::GetTileLayout(FLSTileLayout & OutTileLayout) const
{
	FBox Bounds = GetBounds();
	if (Bounds.IsValid)
	{
		// Determine the bottom-left-most tile that contains the min position (when accounting for overlap)
		const FVector MinPosition = Bounds.Min + TileOverlap;
		OutTileLayout.BottomLeftX = FMath::FloorToInt(MinPosition.X / LSystemSpawner->TileSize);
		OutTileLayout.BottomLeftY = FMath::FloorToInt(MinPosition.Y / LSystemSpawner->TileSize);

		// Determine the total number of tiles along each active axis
		const FVector MaxPosition = Bounds.Max - TileOverlap;
		const int32 MaxXIdx = FMath::FloorToInt(MaxPosition.X / LSystemSpawner->TileSize);
		const int32 MaxYIdx = FMath::FloorToInt(MaxPosition.Y / LSystemSpawner->TileSize);

		OutTileLayout.NumTilesX = (MaxXIdx - OutTileLayout.BottomLeftX) + 1;
		OutTileLayout.NumTilesY = (MaxYIdx - OutTileLayout.BottomLeftY) + 1;

		OutTileLayout.HalfHeight = Bounds.GetExtent().Z;
	}
}

bool ULSystemComponent::ExecuteSimulation(TArray<FDesiredLSysInstance>& OutFoliageInstances)
{
#if WITH_EDITOR
	UE_LOG(LogTemp,Log,TEXT("Went in Execute simul"));

	FBodyInstance* BoundsBodyInstance = GetBoundsBodyInstance();
	if (LSystemSpawner)
	{
		// Establish the counter used to check if the user has canceled the simulation
		FThreadSafeCounter LastCancel;
		const int32 LastCanelInit = LastCancel.GetValue();
		FThreadSafeCounter* LastCancelPtr = &LastCancel;
		
		// Establish basic info about the tiles
		const float TileSize = LSystemSpawner->TileSize;
		const FVector WorldPosition = GetWorldPosition();
		FLSTileLayout TileLayout;
		GetTileLayout(TileLayout);

		LSystemSpawner->SimulateIfNeeded();

		TArray<TFuture< TArray<FDesiredLSysInstance>* >> Futures;
		for (int32 X = 0; X < TileLayout.NumTilesX; ++X)
		{
			for (int32 Y = 0; Y < TileLayout.NumTilesY; ++Y)
			{
				// We have to get the simulated tiles and create new ones to build on main thread
				const ULSystemTile* Tile = LSystemSpawner->GetRandomTile(X + TileLayout.BottomLeftX, Y + TileLayout.BottomLeftY);
				if (Tile == nullptr)	
				{
					// Simulation was either canceled or failed
					return false;
				}

				// From the pool of simulated tiles, pick the neighbors of this tile
				const ULSystemTile* RightTile = (X + 1 < TileLayout.NumTilesX) ? LSystemSpawner->GetRandomTile(X + TileLayout.BottomLeftX + 1, Y + TileLayout.BottomLeftY) : nullptr;
				const ULSystemTile* TopTile = (Y + 1 < TileLayout.NumTilesY) ? LSystemSpawner->GetRandomTile(X + TileLayout.BottomLeftX, Y + TileLayout.BottomLeftY + 1) : nullptr;
				const ULSystemTile* TopRightTile = (RightTile && TopTile) ? LSystemSpawner->GetRandomTile(X + TileLayout.BottomLeftX + 1, Y + TileLayout.BottomLeftY + 1) : nullptr;

				// Create a temp tile that will contain the composite contents of the tile after accounting for overlap
				ULSystemTile* CompositeTile = LSystemSpawner->CreateTempTile();

				Futures.Add(Async<TArray<FDesiredLSysInstance>*>(EAsyncExecution::ThreadPool, [=]()
				{
					if (LastCancelPtr->GetValue() != LastCanelInit)
					{
						// The counter has changed since we began, meaning the user canceled the operation
						return new TArray<FDesiredLSysInstance>();
					}

					//@todo proc foliage: Determine the composite contents of the tile (including overlaps) without copying everything to a temp tile

					// Copy the base tile contents
					const FBox2D BaseTile = GetTileRegion(X, Y, TileSize, TileOverlap);
					Tile->CopyInstancesToTile(CompositeTile, BaseTile, FTransform::Identity, TileOverlap);


					if (RightTile)
					{
						// Add instances from the right overlapping tile
						const FBox2D RightBox(FVector2D(0.f, BaseTile.Min.Y), FVector2D(TileOverlap, BaseTile.Max.Y));
						const FTransform RightTM(FVector(TileSize, 0.f, 0.f));
						RightTile->CopyInstancesToTile(CompositeTile, RightBox, RightTM, TileOverlap);
					}

					if (TopTile)
					{
						// Add instances from the top overlapping tile
						const FBox2D TopBox(FVector2D(BaseTile.Min.X, -TileOverlap), FVector2D(BaseTile.Max.X, TileOverlap));
						const FTransform TopTM(FVector(0.f, TileSize, 0.f));
						TopTile->CopyInstancesToTile(CompositeTile, TopBox, TopTM, TileOverlap);
					}

					if (TopRightTile)
					{
						// Add instances from the top-right overlapping tile
						const FBox2D TopRightBox(FVector2D(-TileOverlap, -TileOverlap), FVector2D(TileOverlap, TileOverlap));
						const FTransform TopRightTM(FVector(TileSize, TileSize, 0.f));
						TopRightTile->CopyInstancesToTile(CompositeTile, TopRightBox, TopRightTM, TileOverlap);
					}

					const FVector OrientedOffset = FVector(X, Y, 0.f) * TileSize;
					const FTransform TileTM(OrientedOffset + WorldPosition);

					TArray<FDesiredLSysInstance>* DesiredInstances = new TArray<FDesiredLSysInstance>();
					CompositeTile->ExtractDesiredInstances(*DesiredInstances, TileTM, ProceduralGuid, TileLayout.HalfHeight, BoundsBodyInstance);

					return DesiredInstances;
					
				})
				);
			}
		}

		const FText StatusMessage = LOCTEXT("PlaceProceduralFoliage", "Placing ProceduralFoliage...");
		const FText CancelMessage = LOCTEXT("PlaceProceduralFoliageCancel", "Canceling ProceduralFoliage...");
		GWarn->BeginSlowTask(StatusMessage, true, true);


		int32 FutureIdx = 0;
		bool bCancelled = false;
		for (int X = 0; X < TileLayout.NumTilesX; ++X)
		{
			for (int Y = 0; Y < TileLayout.NumTilesY; ++Y)
			{
				bool bFirstTime = true;
				while (Futures[FutureIdx].WaitFor(FTimespan::FromMilliseconds(100.0)) == false || bFirstTime)
				{
					if (GWarn->ReceivedUserCancel() && bCancelled == false)
					{
						// Increment the thread-safe counter. Tiles compare against the original count and cancel if different.
						LastCancel.Increment();
						bCancelled = true;
					}

					if (bCancelled)
					{
						GWarn->StatusUpdate(Y + X * TileLayout.NumTilesY, TileLayout.NumTilesX * TileLayout.NumTilesY, CancelMessage);
					}
					else
					{
						GWarn->StatusUpdate(Y + X * TileLayout.NumTilesY, TileLayout.NumTilesX * TileLayout.NumTilesY, StatusMessage);
					}

					bFirstTime = false;
				}

				TArray<FDesiredLSysInstance> DesiredInstances = *Futures[FutureIdx++].Get();
				
				OutFoliageInstances.Append(DesiredInstances);
				//delete DesiredInstances;
			}
		}

		GWarn->EndSlowTask();

		return !bCancelled;
	}
#endif
	return false;
}


void ULSystemComponent::PostEditImport()
{
	// The Guid should always be unique
	ProceduralGuid = FGuid::NewGuid();
}

bool ULSystemComponent::GenerateProceduralContent(TArray<FDesiredLSysInstance>& OutLSystemTypes)
{
#if WITH_EDITOR
	
	if (ExecuteSimulation(OutLSystemTypes))
	{
		RemoveProceduralContent();
		return true;
	}

#endif

	return false;
}

void ULSystemComponent::RemoveProceduralContent()
{
#if WITH_EDITOR
	for(ALSystemFoliage* LSA : CastChecked<ALSystemVolume>(SpawningVolume)->FoliageActors)
	{
		LSA->Destroy();
	}
#endif
}

bool ULSystemComponent::HasSpawnedAnyInstances()
{

	//TODO
	bool bHasSpawnedInstances = false;

	return bHasSpawnedInstances;
}

FVector ULSystemComponent::GetWorldPosition() const
{
	FBox Bounds = GetBounds();
	if (!Bounds.IsValid || LSystemSpawner == nullptr)
	{
		return FVector::ZeroVector;
	}
	
	FLSTileLayout TileLayout;
	GetTileLayout(TileLayout);

	const float TileSize = LSystemSpawner->TileSize;
	return FVector(TileLayout.BottomLeftX * TileSize, TileLayout.BottomLeftY * TileSize, Bounds.GetCenter().Z);
}

#undef LOCTEXT_NAMESPACE