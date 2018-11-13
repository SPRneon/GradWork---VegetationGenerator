//Watteeuw Zeno, Grad Work, Howest DAE, 2018

#include "SVegGenEdit.h"
#include "Fonts/SlateFontInfo.h"
#include "Modules/ModuleManager.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Styling/SlateTypes.h"
#include "SlateOptMacros.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SWrapBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Framework/MultiBox/MultiBoxDefs.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "EditorStyleSet.h"
#include "EditorModeManager.h"
#include "EditorModes.h"

#include "VegGenEditActions.h"
#include "VegetationGeneratorPluginEdMode.h"
#include "IIntroTutorials.h"
#include "Widgets/Input/SNumericEntryBox.h"
//#include "SFoliagePalette.h"
#include "SHeader.h"
#include "SSeparator.h"
#include "SErrorText.h"

#define LOCTEXT_NAMESPACE "VegGenEd_Mode"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SVegGenEdit::Construct(const FArguments& InArgs)
{
	VegGenEditMode = (FVegetationGeneratorPluginEdMode*)GLevelEditorModeTools().GetActiveMode(FVegetationGeneratorPluginEdMode::EM_VegetationGeneratorPluginEdModeId);

	///->replace by veggeneditor mode
	//IIntroTutorials& IntroTutorials = FModuleManager::LoadModuleChecked<IIntroTutorials>(TEXT("IntroTutorials"));
	///See if tutorial necesarry -> nice to have

	FMargin StandardPadding(6.f, 3.f);
	FMargin StandardLeftPadding(6.f, 3.f, 3.f, 3.f);
	FMargin StandardRightPadding(3.f, 3.f, 6.f, 3.f);

	//FSlateFontInfo StandardFont = FEditorStyle::GetFontStyle(TEXT("PropertyWindow.NormalFont"));

	const FText BlankText = LOCTEXT("Blank", "");

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0, 0, 0, 5)
		[
			SAssignNew(ErrorText, SErrorText)
		]
		+ SVerticalBox::Slot()
		.Padding(0)
		[
			
			SNew(SVerticalBox)
			.IsEnabled(this, &SVegGenEdit::IsVegGenEditorEnabled)

			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
			.Padding(1.f,5.f,0.f,5.f)
			[
				BuildToolBar()
			]
			
		]
		]
	
	];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

TSharedRef<SWidget> SVegGenEdit::BuildToolBar()
{
	FToolBarBuilder Toolbar(VegGenEditMode->UICommandList, FMultiBoxCustomization::None, nullptr, Orient_Vertical);
	Toolbar.SetLabelVisibility(EVisibility::Collapsed);
	Toolbar.SetStyle(&FEditorStyle::Get(), "VegGenToolbar");
	{
		Toolbar.AddToolBarButton(FVegGenEditCommands::Get().SetPaint);
		Toolbar.AddToolBarButton(FVegGenEditCommands::Get().SetReapplySettings);
		Toolbar.AddToolBarButton(FVegGenEditCommands::Get().SetSelect);
		Toolbar.AddToolBarButton(FVegGenEditCommands::Get().SetLassoSelect);
		Toolbar.AddToolBarButton(FVegGenEditCommands::Get().SetPaintBucket);
	}
		return
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			[
				SNew(SBorder)
				.HAlign(HAlign_Center)
				.Padding(0)
				.BorderImage(FEditorStyle::GetBrush("NoBorder"))
				.IsEnabled(FSlateApplication::Get().GetNormalExecutionAttribute())
				[
					Toolbar.MakeWidget()
				]
			]
		];
}

bool SVegGenEdit::IsVegGenEditorEnabled() const
{
	ErrorText->SetError(GetVegGenEditorErrorText());
	return VegGenEditMode->IsEditingEnabled();

}

FText SVegGenEdit::GetVegGenEditorErrorText() const
{
	EVegGenEditingState EditState = VegGenEditMode->GetEditingState();


	switch (EditState)
	{
	case EVegGenEditingState::SIEWorld: return LOCTEXT("IsSimulatingError_VegEdit", "Can't edit foliage while simulating!");
	case EVegGenEditingState::PIEWorld: return LOCTEXT( "IsPIEError_VegEdit", "Can't edit foliage in PIE!");
	case EVegGenEditingState::Enabled: return FText::GetEmpty();
	default: checkNoEntry();
	}

	return FText::GetEmpty();
}



 

#undef LOCTEXT_NAMESPACE
