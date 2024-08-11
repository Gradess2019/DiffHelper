// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperCommitPanel.h"
#include "DiffHelperTypes.h"
#include "DiffHelperUtils.h"
#include "SlateOptMacros.h"

#include "UI/DiffHelperTabController.h"
#include "UI/DiffHelperTabModel.h"
#include "UI/SDiffHelperCommitContextMenu.h"
#include "UI/SDiffHelperCommitItem.h"

#define LOCTEXT_NAMESPACE "DiffHelper"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperCommitPanel::Construct(const FArguments& InArgs)
{
	if (!ensure(InArgs._Controller.IsValid()) || !ensure(IsValid(InArgs._Controller->GetModel()))) { return; }

	Controller = InArgs._Controller;

	Controller->OnModelUpdated().AddRaw(this, &SDiffHelperCommitPanel::OnModelUpdated);

	if (InArgs._Commits.Num() > 0)
	{
		Commits = UDiffHelperUtils::ConvertToShared(InArgs._Commits);
	}
	else
	{
		Commits = UDiffHelperUtils::ConvertToShared(Controller->GetModel()->SelectedDiffItem.Commits);
	}

	FToolMenuContext MenuContext(Controller->GetCommitPanelCommands());

	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(SBorder)
			.Padding(0)
			.BorderImage(FAppStyle::Get().GetBrush("NoBorder"))
			[
				UToolMenus::Get()->GenerateWidget("DiffHelper.CommitPanel.Toolbar", MenuContext)
			]
		]
		+ SVerticalBox::Slot()
		.FillHeight(1.f)
		[
			SAssignNew(CommitList, SListView<TSharedPtr<FDiffHelperCommit>>)
			.ListItemsSource(&Commits)
			.OnGenerateRow(this, &SDiffHelperCommitPanel::OnGenerateRow)
			.OnSelectionChanged(this, &SDiffHelperCommitPanel::OnSelectionChanged)
			.OnContextMenuOpening(this, &SDiffHelperCommitPanel::OnContextMenuOpening)
			.HeaderRow
			(
				SNew(SHeaderRow)
				+ SHeaderRow::Column(SDiffHelperCommitPanelConstants::HashColumnId)
				.DefaultLabel(LOCTEXT("HashColumnLabel", "Hash"))
				.FillWidth(0.05f)
				+ SHeaderRow::Column(SDiffHelperCommitPanelConstants::MessageColumnId)
				.DefaultLabel(LOCTEXT("MessageColumnLabel", "Message"))
				.FillWidth(0.5f)
				+ SHeaderRow::Column(SDiffHelperCommitPanelConstants::AuthorColumnId)
				.DefaultLabel(LOCTEXT("AuthorColumnLabel", "Author"))
				.FillWidth(0.1f)
				+ SHeaderRow::Column(SDiffHelperCommitPanelConstants::DateColumnId)
				.DefaultLabel(LOCTEXT("DateColumnLabel", "Date"))
				.FillWidth(0.25f)
			)
		]
	];
}

SDiffHelperCommitPanel::~SDiffHelperCommitPanel()
{
	if (Controller.IsValid() && IsValid(Controller->GetModel()))
	{
		Controller->OnModelUpdated().RemoveAll(this);
	}
}

TSharedPtr<SWidget> SDiffHelperCommitPanel::OnContextMenuOpening()
{
	return SNew(SDiffHelperCommitContextMenu)
		.Controller(Controller);
}

TSharedRef<ITableRow> SDiffHelperCommitPanel::OnGenerateRow(TSharedPtr<FDiffHelperCommit> InItem, const TSharedRef<STableViewBase>& InOwnerTable)
{
	return SNew(SDiffHelperCommitItem, InOwnerTable)
		.Controller(Controller)
		.Item(InItem);
}

void SDiffHelperCommitPanel::OnSelectionChanged(TSharedPtr<FDiffHelperCommit> InCommit, ESelectInfo::Type InSelectInfo)
{
	auto SelectedCommits = CommitList->GetSelectedItems();
	SelectedCommits.Sort([&](const TSharedPtr<FDiffHelperCommit>& A, const TSharedPtr<FDiffHelperCommit>& B)
	{
		return Commits.IndexOfByKey(A) > Commits.IndexOfByKey(B);
	});

	Controller->SetSelectedCommits(SelectedCommits);
}

void SDiffHelperCommitPanel::OnModelUpdated()
{
	Commits = UDiffHelperUtils::ConvertToShared(Controller->GetModel()->SelectedDiffItem.Commits);
	CommitList->RequestListRefresh();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
#undef LOCTEXT_NAMESPACE
