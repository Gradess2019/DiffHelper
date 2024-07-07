// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/FDiffHelperCommitPanelToolbar.h"

#include "DiffHelperCommands.h"


void FDiffHelperCommitPanelToolbar::RegisterMenu()
{
	const auto& Commands = FDiffHelperCommands::Get();
	
	UToolMenu* Menu = UToolMenus::Get()->RegisterMenu("DiffHelper.CommitPanel.Toolbar", NAME_None, EMultiBoxType::ToolBar);
	Menu->SetStyleSet(&FDiffHelperStyle::Get());
	Menu->StyleName = "CommitPanelToolbar";
	
	FToolMenuSection& Section = Menu->AddSection("Content");

	auto& DiffAgainstTargetEntry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.DiffAgainstTarget));
	DiffAgainstTargetEntry.Icon = FSlateIcon(FDiffHelperStyle::GetStyleSetName(), "DiffHelper.HorizontalOutwardArrow");
	
	auto& DiffSelectedCommitsEntry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.DiffSelectedCommits));
	DiffSelectedCommitsEntry.Icon = FSlateIcon(FDiffHelperStyle::GetStyleSetName(), "DiffHelper.VerticalOutwardArrow");

	auto& DiffSelectedCommitAgainstNext = Section.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.DiffSelectedCommitAgainstNext));
	DiffSelectedCommitAgainstNext.Icon = FSlateIcon(FDiffHelperStyle::GetStyleSetName(), "DiffHelper.ArrowUp");

	auto& DiffSelectedCommitAgainstPrevious = Section.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.DiffSelectedCommitAgainstPrevious));
	DiffSelectedCommitAgainstPrevious.Icon = FSlateIcon(FDiffHelperStyle::GetStyleSetName(), "DiffHelper.ArrowDown");

	auto& DiffSelectedCommitAgainstNewest = Section.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.DiffSelectedCommitAgainstNewest));
	DiffSelectedCommitAgainstNewest.Icon = FSlateIcon(FDiffHelperStyle::GetStyleSetName(), "DiffHelper.DoubleArrowUp");
	
	auto& DiffSelectedCommitAgainstOldest = Section.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.DiffSelectedCommitAgainstOldest));
	DiffSelectedCommitAgainstOldest.Icon = FSlateIcon(FDiffHelperStyle::GetStyleSetName(), "DiffHelper.DoubleArrowDown");
}

