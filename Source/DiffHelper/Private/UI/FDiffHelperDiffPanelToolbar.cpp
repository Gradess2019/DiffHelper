// Copyright 2024 Gradess Games. All Rights Reserved.

#include "UI/FDiffHelperDiffPanelToolbar.h"
#include "DiffHelperCommands.h"

void FDiffHelperDiffPanelToolbar::RegisterMenu()
{
	const auto& Commands = FDiffHelperCommands::Get();
	
	UToolMenu* Menu = UToolMenus::Get()->RegisterMenu("DiffHelper.DiffPanel.Toolbar", NAME_None, EMultiBoxType::ToolBar);
	Menu->StyleName = "AssetEditorToolbar";
	
	FToolMenuSection& Section = Menu->AddSection("Content");

	auto& GroupByDirectoryEntry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.GroupByDirectory));
	GroupByDirectoryEntry.Icon = FSlateIcon(FDiffHelperStyle::GetStyleSetName(), "DiffHelper.Directory");

	auto& ExpandAllEntry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.ExpandAll));
	ExpandAllEntry.Icon = FSlateIcon(FDiffHelperStyle::GetStyleSetName(), "DiffHelper.ExpandAll");

	auto& CollapseAllEntry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.CollapseAll));
	CollapseAllEntry.Icon = FSlateIcon(FDiffHelperStyle::GetStyleSetName(), "DiffHelper.CollapseAll");
}
