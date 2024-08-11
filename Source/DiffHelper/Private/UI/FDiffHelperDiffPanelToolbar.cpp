// Copyright 2024 Gradess Games. All Rights Reserved.

#include "UI/FDiffHelperDiffPanelToolbar.h"
#include "DiffHelperCommands.h"

void FDiffHelperDiffPanelToolbar::RegisterMenu()
{
	const auto& Commands = FDiffHelperCommands::Get();
	
	UToolMenu* Menu = UToolMenus::Get()->RegisterMenu("DiffHelper.DiffPanel.Toolbar", NAME_None, EMultiBoxType::ToolBar);
	Menu->SetStyleSet(&FDiffHelperStyle::Get());
	Menu->StyleName = "CommonToolbar";
	
	FToolMenuSection& Section = Menu->AddSection("Content");

	auto& GroupByDirectoryEntry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.GroupByDirectory));
	// GroupByDirectoryEntry.UserInterfaceActionType = EUserInterfaceActionType::ToggleButton;
	GroupByDirectoryEntry.Icon = FSlateIcon(FDiffHelperStyle::GetStyleSetName(), "DiffHelper.GroupByDirectory");

	auto& ExpandAllEntry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.ExpandAll));
	ExpandAllEntry.Icon = FSlateIcon(FDiffHelperStyle::GetStyleSetName(), "DiffHelper.ExpandAll");

	auto& CollapseAllEntry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.CollapseAll));
	CollapseAllEntry.Icon = FSlateIcon(FDiffHelperStyle::GetStyleSetName(), "DiffHelper.CollapseAll");
}
