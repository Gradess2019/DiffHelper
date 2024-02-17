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
			.WidthOverride(350.f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				[
					SAssignNew(SourceBranchPicker, SDiffHelperBranchPicker)
					.Controller(InArgs._Controller)
					.Hint(LOCTEXT("DiffHelperSourceBranchHint", "Select source branch..."))
				]
				+ SVerticalBox::Slot()
				[
					SAssignNew(TargetBranchPicker, SDiffHelperBranchPicker)
					.Controller(InArgs._Controller)
					.Hint(LOCTEXT("DiffHelperTargetBranchHint", "Select target branch..."))
				]
				// add "Show diff" button here
				+ SVerticalBox::Slot()
				[
					SNew(SButton)
					.IsEnabled(this, &SDiffHelperPickerPanel::CanShowDiff)
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

bool SDiffHelperPickerPanel::CanShowDiff() const
{
	const auto& SourceBranch = SourceBranchPicker->GetSelectedBranch();
	const auto& TargetBranch = TargetBranchPicker->GetSelectedBranch();

	return SourceBranch.IsValid() && TargetBranch.IsValid() && SourceBranch != TargetBranch;
}

#undef LOCTEXT_NAMESPACE

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
