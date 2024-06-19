// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperCommitPanelToolbar.h"

#include "DiffHelperStyle.h"
#include "SlateOptMacros.h"

#include "UI/DiffHelperTabController.h"
#include "UI/DiffHelperTabModel.h"

#define LOCTEXT_NAMESPACE "DiffHelper"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperCommitPanelToolbar::Construct(const FArguments& InArgs)
{
	if (!ensure(InArgs._Controller.IsValid()) || !ensure(IsValid(InArgs._Controller->GetModel()))) { return; }
	Controller = InArgs._Controller;
	Model = Controller->GetModel();

	Controller->OnModelUpdated().AddSP(this, &SDiffHelperCommitPanelToolbar::OnModelUpdated);
	
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
			.IsEnabled(this, &SDiffHelperCommitPanelToolbar::IsDiffSelectedButtonEnabled)
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
			.IsEnabled(this, &SDiffHelperCommitPanelToolbar::HasNextCommit)
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
			.IsEnabled(this, &SDiffHelperCommitPanelToolbar::HasPreviousCommit)
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
			.IsEnabled(this, &SDiffHelperCommitPanelToolbar::HasNextCommit)
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
			.IsEnabled(this, &SDiffHelperCommitPanelToolbar::HasPreviousCommit)
			.Content()
			[
				SNew(SImage)
				.Image(FDiffHelperStyle::Get().GetBrush("DiffHelper.DoubleArrowDown"))
			]
		]
	];
}

int32 SDiffHelperCommitPanelToolbar::GetCommitIndex(const FDiffHelperCommit& InCommit) const
{
	if (!ensure(Model.IsValid())) { return INDEX_NONE; }

	const auto& Commits = Model->SelectedDiffItem.Commits;
	return Commits.IndexOfByPredicate([&InCommit](const FDiffHelperCommit& Commit)
	{
		return Commit.Revision == InCommit.Revision;
	});
}

void SDiffHelperCommitPanelToolbar::OnModelUpdated()
{
	if (!ensure(Model.IsValid())) { return; }
}

bool SDiffHelperCommitPanelToolbar::IsDiffSelectedButtonEnabled() const
{
	const auto& Data = Model->CommitPanelData;
	return Data.SelectedCommits.Num() == 2;
}

bool SDiffHelperCommitPanelToolbar::HasNextCommit() const
{
	const auto& Data = Model->CommitPanelData;
	if (Data.SelectedCommits.Num() != 1) { return false; }

	const auto Index = GetCommitIndex(*Data.SelectedCommits[0]);
	return Index > 0;
}

bool SDiffHelperCommitPanelToolbar::HasPreviousCommit() const
{
	const auto& Data = Model->CommitPanelData;
	if (Data.SelectedCommits.Num() != 1) { return false; }

	const auto Index = GetCommitIndex(*Data.SelectedCommits[0]);
	return Index < Model->SelectedDiffItem.Commits.Num() - 1;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
