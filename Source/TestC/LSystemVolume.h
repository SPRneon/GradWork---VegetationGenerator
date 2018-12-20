// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/Volume.h"
#include "LSystemVolume.generated.h"

/**
 * 
 */
class ULSystemComponent;

UCLASS()
class TESTC_API ALSystemVolume : public AVolume
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(Category = Lindenmayer, VisibleAnywhere, BlueprintReadOnly)
	ULSystemComponent* LSystemComponent;
	
	#if WITH_EDITOR

	// UObject interface
	virtual void PostEditImport() override;

	virtual bool GetReferencedContentObjects(TArray<UObject*>& Objects) const override;
#endif
};