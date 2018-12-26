// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorFactories/ActorFactory.h"
#include "LSystemFoliageFactory.generated.h"


/**
 * 
 */
UCLASS()
class TESTC_API ULSystemFoliageFactory : public UActorFactory
{
	GENERATED_UCLASS_BODY()
	
	// UActorFactory interface
	virtual bool PreSpawnActor(UObject* Asset, FTransform& InOutLocation);
	virtual AActor* SpawnActor(UObject* Asset, ULevel* InLevel, const FTransform& Transform, EObjectFlags ObjectFlags, const FName Name) override;
	virtual void PostSpawnActor( UObject* Asset, AActor* NewActor) override;
	virtual void PostCreateBlueprint( UObject* Asset, AActor* CDO ) override;
	// End of UActorFactory interface
	
	
};
