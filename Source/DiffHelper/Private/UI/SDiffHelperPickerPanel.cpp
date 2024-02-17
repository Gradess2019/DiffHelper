// Copyright 2024 Gradess Games. All Rights Reserved.


#include "SDiffHelperPickerPanel.h"
#include "SlateOptMacros.h"
#include "UI/SDiffHelperBranchPicker.h"

#define LOCTEXT_NAMESPACE "DiffHelper"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperPickerPanel::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		  .HAlign(HAlign_Center)
		  .VAlign(VAlign_Center)
		[
			SNew(SBox)
			.WidthOverride(250.f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				[
					SNew(SDiffHelperBranchPicker)
					.Controller(InArgs._Controller)
					.Hint(LOCTEXT("DiffHelperSourceBranchHint", "Select source branch..."))
				]
				+ SVerticalBox::Slot()
				[
					SNew(SDiffHelperBranchPicker)
					.Controller(InArgs._Controller)
					.Hint(LOCTEXT("DiffHelperTargetBranchHint", "Select target branch..."))
				]
				// add "Show diff" button here
				+ SVerticalBox::Slot()
				[
					SNew(SButton)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("DiffHelperShowDiffButton", "Show diff"))
						.Justification(ETextJustify::Center)
					]
				]
			]
		]
	];
}

#undef LOCTEXT_NAMESPACE

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
