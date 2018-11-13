// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "VegetationGeneratorPluginEdModeToolkit.h"
#include "VegetationGeneratorPluginEdMode.h"
#include "Engine/Selection.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "EditorModeManager.h"
#include "SVegGenEdit.h"

#define LOCTEXT_NAMESPACE "FVegetationGeneratorPluginEdModeToolkit"

void FVegetationGeneratorPluginEdModeToolkit::RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager)
{

}

void FVegetationGeneratorPluginEdModeToolkit::UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager)
{

}

void FVegetationGeneratorPluginEdModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost)
{
	struct Locals
	{
		static bool IsWidgetEnabled()
		{
			return GEditor->GetSelectedActors()->Num() != 0;
		}

		static FReply OnButtonClick(FVector InOffset)
		{
			USelection* SelectedActors = GEditor->GetSelectedActors();

			// Let editor know that we're about to do something that we want to undo/redo
			GEditor->BeginTransaction(LOCTEXT("MoveActorsTransactionName", "MoveActors"));

			// For each selected actor
			for (FSelectionIterator Iter(*SelectedActors); Iter; ++Iter)
			{
				if (AActor* LevelActor = Cast<AActor>(*Iter))
				{
					// Register actor in opened transaction (undo/redo)
					LevelActor->Modify();
					// Move actor to given location
					LevelActor->TeleportTo(LevelActor->GetActorLocation() + InOffset, FRotator(0, 0, 0));
				}
			}

			// We're done moving actors so close transaction
			GEditor->EndTransaction();

			return FReply::Handled();
		}

		static TSharedRef<SWidget> MakeButton(FText InLabel, const FVector InOffset)
		{
			return SNew(SButton)
				.Text(InLabel)
				.OnClicked_Static(&Locals::OnButtonClick, InOffset);
		}
	};

	VegGenEdtWidget = SNew(SVegGenEdit);
		
	FModeToolkit::Init(InitToolkitHost);
}

FName FVegetationGeneratorPluginEdModeToolkit::GetToolkitFName() const
{
	return FName("VegetationGeneratorPluginEdMode");
}

FText FVegetationGeneratorPluginEdModeToolkit::GetBaseToolkitName() const
{
	return NSLOCTEXT("VegetationGeneratorPluginEdModeToolkit", "DisplayName", "VegetationGeneratorPluginEdMode Tool");
}

class FEdMode* FVegetationGeneratorPluginEdModeToolkit::GetEditorMode() const
{
	return GLevelEditorModeTools().GetActiveMode(FVegetationGeneratorPluginEdMode::EM_VegetationGeneratorPluginEdModeId);
}

TSharedPtr<SWidget> FVegetationGeneratorPluginEdModeToolkit::GetInlineContent() const
{
	return VegGenEdtWidget;
}

#undef LOCTEXT_NAMESPACE
