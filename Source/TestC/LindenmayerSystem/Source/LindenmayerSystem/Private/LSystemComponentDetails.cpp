// Fill out your copyright notice in the Description page of Project Settings.

#include "LSystemComponentDetails.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "PropertyHandle.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "DetailCategoryBuilder.h"
#include "LSystemVolume.h"
#include "LSystemFoliage.h"
#include "LSystemFoliageSpawner.h"
//#include "VegetationGeneratorPluginEdMode.h"
#include "ScopedTransaction.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "LSystemComponent.h"

#define LOCTEXT_NAMESPACE "LSystemComponentDetails"

TSharedRef<IDetailCustomization> FLSystemComponentDetails::MakeInstance()
{
	return MakeShareable(new FLSystemComponentDetails());

}
//
void FLSystemComponentDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	const FName LSystemCategoryName("Lindenmayer");
	IDetailCategoryBuilder& LSystemCategory = DetailBuilder.EditCategory(LSystemCategoryName);
	
	const FText ResimulateText = LOCTEXT("ResimulateButtonText", "Resimulate");

	TArray< TWeakObjectPtr<UObject> > ObjectsBeingCustomized;
	DetailBuilder.GetObjectsBeingCustomized(ObjectsBeingCustomized);

	for (TWeakObjectPtr<UObject>& Object : ObjectsBeingCustomized)
	{
		ULSystemComponent* Component = Cast<ULSystemComponent>(Object.Get());
		if (ensure(Component))
		{
			SelectedComponents.Add(Component);
		}
	}

	TArray<TSharedRef<IPropertyHandle>> AllProperties;
	bool bSimpleProperties = true;
	bool bAdvancedProperties = false;
	// Add all properties in the category in order
	LSystemCategory.GetDefaultProperties(AllProperties, bSimpleProperties, bAdvancedProperties);
	for (auto& Property : AllProperties)
	{
		LSystemCategory.AddProperty(Property);
	}


	
	FDetailWidgetRow& NewRow = LSystemCategory.AddCustomRow(ResimulateText);

	NewRow.ValueContent()
		.MaxDesiredWidth(120.f)
		[
			SNew(SButton)
			.OnClicked(this, &FLSystemComponentDetails::OnResimulateClicked)
			.ToolTipText(this, &FLSystemComponentDetails::GetResimulateTooltipText)
			.IsEnabled(this, &FLSystemComponentDetails::IsResimulateEnabled)
			[
				SNew(STextBlock)
				.Font(IDetailLayoutBuilder::GetDetailFont())
				.Text(ResimulateText)
			]
		];
}

FReply FLSystemComponentDetails::OnResimulateClicked()
{
	

	for (auto& Component : SelectedComponents)
	{
		if (Component.IsValid() && Component->LSystemSpawner)
		{
			FScopedTransaction Transaction(LOCTEXT("Resimulate_Transaction", "LSystem Foliage Simulation"));
			TArray <FDesiredLSysInstance> DesiredFoliageInstances;
			if (Component->GenerateProceduralContent(DesiredFoliageInstances))
			{
				/*UE_LOG(LogTemp,Log,TEXT("Went in generate proc content"));
				FFoliagePaintingGeometryFilter OverrideGeometryFilter;
				OverrideGeometryFilter.bAllowLandscape = Component->bAllowLandscape;
				OverrideGeometryFilter.bAllowStaticMesh = Component->bAllowStaticMesh;
				OverrideGeometryFilter.bAllowBSP = Component->bAllowBSP;
				OverrideGeometryFilter.bAllowFoliage = Component->bAllowFoliage;
				OverrideGeometryFilter.bAllowTranslucent = Component->bAllowTranslucent;


				auto vol = Component->LSystemSpawner->
				ALSystemVolume::SpawnLSystemInstances(Component->GetWorld(), DesiredFoliageInstances, OverrideGeometryFilter);
*/
				// If no instances were spawned, inform the user
				if (!Component->HasSpawnedAnyInstances())
				{
					FNotificationInfo Info(LOCTEXT("NothingSpawned_Notification", "Unable to spawn instances. Ensure a large enough surface exists within the volume."));
					Info.bUseLargeFont = false;
					Info.bFireAndForget = true;
					Info.bUseThrobber = false;
					Info.bUseSuccessFailIcons = true;
					FSlateNotificationManager::Get().AddNotification(Info);
				}
			}
		}
	}
	return FReply::Handled();
}

bool FLSystemComponentDetails::IsResimulateEnabled() const
{
	return true;
}

FText FLSystemComponentDetails::GetResimulateTooltipText() const
{
	FText TooltipText;


	if (TooltipText.IsEmpty())
	{
		TooltipText = LOCTEXT("Resimulate_Tooltip", "Runs the LSYstem foliage spawner simulation. Replaces any existing instances spawned by a previous simulation.");
	}

	return TooltipText;

	
}


#undef LOCTEXT_NAMESPACE