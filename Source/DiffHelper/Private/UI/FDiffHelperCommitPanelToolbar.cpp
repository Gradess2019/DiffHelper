// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/FDiffHelperCommitPanelToolbar.h"

#include "DiffHelperCommands.h"


void FDiffHelperCommitPanelToolbar::RegisterMenu()
{
	const auto& Commands = FDiffHelperCommands::Get();
	
	UToolMenu* Menu = UToolMenus::Get()->RegisterMenu("DiffHelper.CommitPanel.Toolbar", NAME_None, EMultiBoxType::ToolBar);
	// Menu->SetStyleSet(&FDiffHelperStyle::Get());
	
	FToolMenuSection& Section = Menu->AddSection("Content");
	auto& DiffSelectedCommitsEntry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.DiffSelectedCommits));
	DiffSelectedCommitsEntry.Label = FText::GetEmpty();
	DiffSelectedCommitsEntry.Icon = FSlateIcon(FDiffHelperStyle::GetStyleSetName(), "DiffHelper.DoubleHeadedArrow");

	auto& DiffSelectedCommitAgainstNext = Section.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.DiffSelectedCommitAgainstNext));
	DiffSelectedCommitAgainstNext.Label = FText::GetEmpty();
	DiffSelectedCommitAgainstNext.Icon = FSlateIcon(FDiffHelperStyle::GetStyleSetName(), "DiffHelper.ArrowUp");

	auto& DiffSelectedCommitAgainstPrevious = Section.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.DiffSelectedCommitAgainstPrevious));
	DiffSelectedCommitAgainstPrevious.Label = FText::GetEmpty();
	DiffSelectedCommitAgainstPrevious.Icon = FSlateIcon(FDiffHelperStyle::GetStyleSetName(), "DiffHelper.ArrowDown");

	auto& DiffSelectedCommitAgainstNewest = Section.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.DiffSelectedCommitAgainstNewest));
	DiffSelectedCommitAgainstNewest.Label = FText::GetEmpty();
	DiffSelectedCommitAgainstNewest.Icon = FSlateIcon(FDiffHelperStyle::GetStyleSetName(), "DiffHelper.DoubleArrowUp");
	
	auto& DiffSelectedCommitAgainstOldest = Section.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.DiffSelectedCommitAgainstOldest));
	DiffSelectedCommitAgainstOldest.Label = FText::GetEmpty();
	DiffSelectedCommitAgainstOldest.Icon = FSlateIcon(FDiffHelperStyle::GetStyleSetName(), "DiffHelper.DoubleArrowDown");
}

