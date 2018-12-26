// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/WeakObjectPtr.h"
#include "Input/Reply.h"
#include "IDetailCustomization.h"

/**
 * 
 */

class IDetailLayoutBuilder;
class ALSystemFoliageSpawner;


class FLSystemComponentDetails : public IDetailCustomization
{
public:
	virtual ~FLSystemComponentDetails(){};

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	/** IDetailCustomization interface */
	virtual void CustomizeDetails( IDetailLayoutBuilder& DetailBuilder ) override;

	private:
	FReply OnResimulateClicked();
	bool IsResimulateEnabled() const;
	FText GetResimulateTooltipText() const;
private:
	TArray< TWeakObjectPtr<class ULSystemComponent> > SelectedComponents;
};
