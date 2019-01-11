// Fill out your copyright notice in the Description page of Project Settings.

#include "LSystemComponentVisualizer.h"
#include "LSystemFoliageSpawner.h"
#include "SceneManagement.h"
#include "LSystemComponent.h"

static const FColor& LsysTileColor = FColor::Yellow;
static const FColor& LSysTileOverlapColor = FColor::Green;


void LSystemComponentVisualizer::DrawVisualization(const UActorComponent* Component, const FSceneView* View, FPrimitiveDrawInterface* PDI)
{
	if (const ULSystemComponent* ProcComponent = Cast<const ULSystemComponent>(Component))
	{
		if (ProcComponent->bShowDebugTiles && ProcComponent->LSystemSpawner)
		{
			const FVector TilesOrigin = ProcComponent->GetWorldPosition();

			const float TileSize = ProcComponent->LSystemSpawner->TileSize;
			const FVector TileSizeV(TileSize, TileSize, 0.f);

			const float TileOverlap = ProcComponent->TileOverlap;
			const FVector TileOverlapV(TileOverlap, TileOverlap, 0.f);
			
			FLSTileLayout TileLayout;
			ProcComponent->GetTileLayout(TileLayout);

			// Draw each tile
			for (int32 X = 0; X < TileLayout.NumTilesX; ++X)
			{
				for (int32 Y = 0; Y < TileLayout.NumTilesY; ++Y)
				{
					const FVector StartOffset(X*TileSize, Y*TileSize, 0.f);
					
					// Draw the tile without overlap
					const FBox BoxInner(TilesOrigin + StartOffset, TilesOrigin + StartOffset + TileSizeV);
					
					DrawWireBox(PDI, BoxInner, LsysTileColor, SDPG_World);

					if (TileOverlap != 0.f)
					{
						// Draw the tile expanded by the overlap amount
						const FBox BoxOuter = BoxInner + (BoxInner.Min - TileOverlapV) + (BoxInner.Max + TileOverlapV);
						DrawWireBox(PDI, BoxOuter, LSysTileOverlapColor, SDPG_World);
					}
				}
			}
			
		}
	}
}

