﻿// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperDiffPanel.h"
#include "DiffHelperStyle.h"
#include "DiffHelperTypes.h"
#include "DiffHelperUtils.h"
#include "UI/DiffHelperTabController.h"
#include "UI/DiffHelperTabModel.h"
#include "UI/SDiffHelperDiffPanelList.h"
#include "UI/SDiffHelperDiffPanelTree.h"
#include "UI/SDiffHelperTreeItem.h"
#include "UI/SDiffHelperDiffItemContextMenu.h"
#include "Styling/ToolBarStyle.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/ITableRow.h"
#include "SlateOptMacros.h"

#define LOCTEXT_NAMESPACE "DiffHelper"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperDiffPanel::Construct(const FArguments& InArgs)
{
	Controller = InArgs._Controller;
	if (!ensure(Controller.IsValid())) { return; }

	Model = Controller->GetModel();
	if (!ensure(Model.IsValid())) { return; }

	DiffList = SNew(SDiffHelperDiffPanelList)
		.Controller(Controller)
		.OnSelectionChanged(this, &SDiffHelperDiffPanel::OnSelectionChanged)
		.OnGenerateRow(this, &SDiffHelperDiffPanel::OnGenerateRow)
		.SortMode(this, &SDiffHelperDiffPanel::GetSortMode)
		.OnSortModeChanged(this, &SDiffHelperDiffPanel::OnSortColumn)
		.OnContextMenuOpening(this, &SDiffHelperDiffPanel::OnContextMenuOpening)
		.CanBroadcastSelectionChanged(this, &SDiffHelperDiffPanel::CanBroadcastSelectionChanged);

	DiffTree = SNew(SDiffHelperDiffPanelTree)
		.Controller(Controller)
		.OnSelectionChanged(this, &SDiffHelperDiffPanel::OnSelectionChanged)
		.OnGenerateRow(this, &SDiffHelperDiffPanel::OnGenerateRow)
		.SortMode(this, &SDiffHelperDiffPanel::GetSortMode)
		.OnSortModeChanged(this, &SDiffHelperDiffPanel::OnSortColumn)
		.OnContextMenuOpening(this, &SDiffHelperDiffPanel::OnContextMenuOpening)
		.CanBroadcastSelectionChanged(this, &SDiffHelperDiffPanel::CanBroadcastSelectionChanged);

	FToolMenuContext MenuContext(Controller->GetDiffPanelCommands());

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(0.f)
		[
			SNew(SBorder)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.BorderImage(FAppStyle::GetBrush("ToolPanel.GroupBorder"))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Fill)
				.AutoHeight()
				[
					SNew(SBorder)
					.Padding(0)
					.BorderImage(FAppStyle::Get().GetBrush("NoBorder"))
					[
						UToolMenus::Get()->GenerateWidget("DiffHelper.DiffPanel.Toolbar", MenuContext)
					]
				]
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Fill)
				.AutoHeight()
				[
					SAssignNew(SearchBox, SSearchBox)
					.HintText(LOCTEXT("SearchBoxHint", "Search the files"))
					.OnTextChanged(this, &SDiffHelperDiffPanel::OnSearchTextChanged)
				]
			]
		]
		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		[
			SNew(SWidgetSwitcher)
			.WidgetIndex(this, &SDiffHelperDiffPanel::GetWidgetIndex)
			+ SWidgetSwitcher::Slot()
			[
				DiffList.ToSharedRef()
			]
			+ SWidgetSwitcher::Slot()
			[
				DiffTree.ToSharedRef()
			]
		]
	];
}

EColumnSortMode::Type SDiffHelperDiffPanel::GetSortMode() const
{
	return Model.IsValid() ? Model->DiffPanelData.SortMode : EColumnSortMode::Ascending;
}

int SDiffHelperDiffPanel::GetWidgetIndex() const
{
	return Model.IsValid() ? Model->DiffPanelData.CurrentWidgetIndex : 0;
}

void SDiffHelperDiffPanel::SyncSelection()
{
	if (!Model->SelectedDiffItem.IsValid())
	{
		return;
	}

	const auto bListSelected = Model->DiffPanelData.CurrentWidgetIndex == SDiffHelperDiffPanelConstants::ListWidgetIndex;
	if (bListSelected)
	{
		const auto& SelectedItems = DiffList->GetSelectedItems();
		if (ensure(SelectedItems.Num() > 0))
		{
			const auto& TreeNode = UDiffHelperUtils::FindItemInTree(Model->DiffPanelData.TreeDiff, SelectedItems[0]);
			if (TreeNode.IsValid())
			{
				DiffTree->SetSelection(TreeNode);
				DiffTree->SetItemExpansion(TreeNode, true);
			}
		}
		
	}
}

void SDiffHelperDiffPanel::OnSearchTextChanged(const FText& InText)
{
	Controller->SetSearchFilter(InText);
	Controller->UpdateItemsData();

	const auto Error = Model->DiffPanelData.SearchFilter->GetFilterErrorText();
	SearchBox->SetError(Error);

	DiffList->RequestListRefresh();
	DiffTree->RequestTreeRefresh();
}

void SDiffHelperDiffPanel::OnSortColumn(EColumnSortPriority::Type InPriority, const FName& InColumnId, EColumnSortMode::Type InSortMode)
{
	Controller->SetSortingMode(InColumnId, InSortMode);

	if (DiffList.IsValid())
	{
		DiffList->RequestListRefresh();
	}

	if (DiffTree.IsValid())
	{
		DiffTree->RequestTreeRefresh();
	}
}

void SDiffHelperDiffPanel::OnSelectionChanged(TSharedPtr<FDiffHelperItemNode> InSelectedItem, ESelectInfo::Type InSelectType)
{
	if (InSelectedItem.IsValid() && InSelectedItem->DiffItem.IsValid())
	{
		Controller->SelectDiffItem(*InSelectedItem->DiffItem);
	}
	else
	{
		Controller->SelectDiffItem(FDiffHelperDiffItem());
	}

	SyncSelection();
	Controller->CallModelUpdated();
}

TSharedRef<ITableRow> SDiffHelperDiffPanel::OnGenerateRow(TSharedPtr<FDiffHelperItemNode> InItem, const TSharedRef<STableViewBase>& InOwnerTable)
{
	auto NewRow = SNew(SDiffHelperTreeItem, InOwnerTable)
		.Controller(Controller)
		.Item(InItem);

	if (InOwnerTable == DiffList)
	{
		NewRow->SetToolTipText(FText::FromString(InItem->DiffItem->Path));
	}

	return NewRow;
}

TSharedPtr<SWidget> SDiffHelperDiffPanel::OnContextMenuOpening()
{
	return SNew(SDiffHelperDiffItemContextMenu)
		.Controller(Controller);
}

bool SDiffHelperDiffPanel::CanBroadcastSelectionChanged(const TSharedPtr<SListView<TSharedPtr<FDiffHelperItemNode>>>& ListView)
{
	if (!ListView.IsValid())
	{
		return false;
	}

	const auto& CurrentWidgetId = Model->DiffPanelData.CurrentWidgetIndex;
	if (ListView == DiffList && CurrentWidgetId == SDiffHelperDiffPanelConstants::ListWidgetIndex)
	{
		return true;
	}
	else if (ListView == DiffTree && CurrentWidgetId == SDiffHelperDiffPanelConstants::TreeWidgetIndex)
	{
		return true;
	}

	return false;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE
