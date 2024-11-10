// Copyright 2024 Gradess Games. All Rights Reserved.

#include "UI/FDiffHelperDiffPanelToolbar.h"
#include "DiffHelperCommands.h"

void FDiffHelperDiffPanelToolbar::RegisterMenu()
{
	const auto& Commands = FDiffHelperCommands::Get();
	
	UToolMenu* Menu = UToolMenus::Get()->RegisterMenu("DiffHelper.DiffPanel.Toolbar", NAME_None, EMultiBoxType::ToolBar);
	Menu->StyleName = "AssetEditorToolbar";
	
	FToolMenuSection& Section = Menu->AddSection("Content");

	auto& OpenAssetEntry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.OpenAsset));
	OpenAssetEntry.Icon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Edit");

	auto& ShowInContentBrowserEntry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.ShowInContentBrowser));
	ShowInContentBrowserEntry.Icon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "SystemWideCommands.FindInContentBrowser.Small");

	auto& OpenLocationEntry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.OpenLocation));
	OpenLocationEntry.Icon = FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentBrowser.ShowInExplorer");

	auto& GroupByDirectoryEntry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.GroupByDirectory));
	GroupByDirectoryEntry.Icon = FSlateIcon(FDiffHelperStyle::GetStyleSetName(), "DiffHelper.Directory");

	auto& ExpandAllEntry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.ExpandAll));
	ExpandAllEntry.Icon = FSlateIcon(FDiffHelperStyle::GetStyleSetName(), "DiffHelper.ExpandAll");

	auto& CollapseAllEntry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(Commands.CollapseAll));
	CollapseAllEntry.Icon = FSlateIcon(FDiffHelperStyle::GetStyleSetName(), "DiffHelper.CollapseAll");
}
