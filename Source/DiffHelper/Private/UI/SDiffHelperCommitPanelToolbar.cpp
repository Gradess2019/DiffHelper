// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperCommitPanelToolbar.h"

#include "DiffHelperStyle.h"
#include "SlateOptMacros.h"

#define LOCTEXT_NAMESPACE "DiffHelper"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperCommitPanelToolbar::Construct(const FArguments& InArgs)
{
	FButtonStyle ButtonStyle = FAppStyle::Get().GetWidgetStyle<FButtonStyle>("NoBorder");
	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SAssignNew(DiffSelectedButton, SButton)
			.ButtonStyle(FDiffHelperStyle::Get(), "CommitPanelToolbarButton")
			.ToolTipText(LOCTEXT("CommitPanelToolbar_DiffSelectedButtonTooltip", "Diff selected commits against each other"))
			// .OnClicked(this, &SDiffHelperCommitPanelToolbar::OnDiffSelectedButtonClicked)
			.Content()
			[
				SNew(SImage)
				.Image(FDiffHelperStyle::Get().GetBrush("DiffHelper.DoubleHeadedArrow"))
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SAssignNew(DiffAgainstNextButton, SButton)
			.ButtonStyle(FDiffHelperStyle::Get(), "CommitPanelToolbarButton")
			.ToolTipText(LOCTEXT("CommitPanelToolbar_DiffAgainstNextButtonTooltip", "Diff selected commit against the next one"))
			// .OnClicked(this, &SDiffHelperCommitPanelToolbar::OnDiffAgainstNextButtonClicked)
			.Content()
			[
				SNew(SImage)
				.Image(FDiffHelperStyle::Get().GetBrush("DiffHelper.ArrowUp"))
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SAssignNew(DiffAgainstPreviousButton, SButton)
			.ButtonStyle(FDiffHelperStyle::Get(), "CommitPanelToolbarButton")
			.ToolTipText(LOCTEXT("CommitPanelToolbar_DiffAgainstPreviousButtonTooltip", "Diff selected commit against the previous one"))
			// .OnClicked(this, &SDiffHelperCommitPanelToolbar::OnDiffAgainstPreviousButtonClicked)
			.Content()
			[
				SNew(SImage)
				.Image(FDiffHelperStyle::Get().GetBrush("DiffHelper.ArrowDown"))
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SAssignNew(DiffAgainstHeadButton, SButton)
			.ButtonStyle(FDiffHelperStyle::Get(), "CommitPanelToolbarButton")
			.ToolTipText(LOCTEXT("CommitPanelToolbar_DiffAgainstHeadButtonTooltip", "Diff selected commit against the head"))
			// .OnClicked(this, &SDiffHelperCommitPanelToolbar::OnDiffAgainstHeadButtonClicked)
			.Content()
			[
				SNew(SImage)
				.Image(FDiffHelperStyle::Get().GetBrush("DiffHelper.DoubleArrowUp"))
			]
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SAssignNew(DiffAgainstOldestButton, SButton)
			.ButtonStyle(FDiffHelperStyle::Get(), "CommitPanelToolbarButton")
			.ToolTipText(LOCTEXT("CommitPanelToolbar_DiffAgainstOldestButtonTooltip", "Diff selected commit against the oldest one"))
			// .OnClicked(this, &SDiffHelperCommitPanelToolbar::OnDiffAgainstOldestButtonClicked)
			.Content()
			[
				SNew(SImage)
				.Image(FDiffHelperStyle::Get().GetBrush("DiffHelper.DoubleArrowDown"))
			]
		]
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
