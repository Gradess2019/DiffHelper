// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperPickerPanel.h"
#include "UI/SDiffHelperBranchPicker.h"
#include "UI/DiffHelperTabController.h"
#include "SlateOptMacros.h"

#include "UI/DiffHelperTabModel.h"

#define LOCTEXT_NAMESPACE "DiffHelper"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperPickerPanel::Construct(const FArguments& InArgs)
{
	Controller = InArgs._Controller;
	ensure(Controller.IsValid());

	OnShowDiff = InArgs._OnShowDiff;

	TSharedPtr<FDiffHelperBranch> CachedSourceBranch = MakeShareable<FDiffHelperBranch>(new FDiffHelperBranch(Controller->GetModel()->SourceBranch));
	TSharedPtr<FDiffHelperBranch> CachedTargetBranch = MakeShareable<FDiffHelperBranch>(new FDiffHelperBranch(Controller->GetModel()->TargetBranch));
	
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
					.Controller(Controller)
					.InitiallySelectedBranch(CachedSourceBranch)
					.Hint(LOCTEXT("DiffHelperSourceBranchHint", "Select source branch..."))
				]
				+ SVerticalBox::Slot()
				[
					SAssignNew(TargetBranchPicker, SDiffHelperBranchPicker)
					.Controller(Controller)
					.InitiallySelectedBranch(CachedTargetBranch)
					.Hint(LOCTEXT("DiffHelperTargetBranchHint", "Select target branch..."))
				]
				// add "Show diff" button here
				+ SVerticalBox::Slot()
				[
					SNew(SButton)
					.OnClicked(this, &SDiffHelperPickerPanel::OnShowDiffClicked)
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

FReply SDiffHelperPickerPanel::OnShowDiffClicked() const
{
	const auto& SourceBranch = SourceBranchPicker->GetSelectedBranch();
	const auto& TargetBranch = TargetBranchPicker->GetSelectedBranch();

	Controller->SetSourceBranch(SourceBranch);
	Controller->SetTargetBranch(TargetBranch);
	Controller->CollectDiff();
	Controller->CallModelUpdated();

	OnShowDiff.ExecuteIfBound();
	
	return FReply::Handled();
}

bool SDiffHelperPickerPanel::CanShowDiff() const
{
	const auto& SourceBranch = SourceBranchPicker->GetSelectedBranch();
	const auto& TargetBranch = TargetBranchPicker->GetSelectedBranch();

	return SourceBranch.IsValid() && TargetBranch.IsValid() && SourceBranch != TargetBranch;
}

#undef LOCTEXT_NAMESPACE

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
