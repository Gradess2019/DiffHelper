// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperCommitPanel.h"
#include "DiffHelperTypes.h"
#include "DiffHelperUtils.h"
#include "SlateOptMacros.h"

#include "UI/DiffHelperTabController.h"
#include "UI/DiffHelperTabModel.h"
#include "UI/SDiffHelperCommitItem.h"

#define LOCTEXT_NAMESPACE "DiffHelper"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperCommitPanel::Construct(const FArguments& InArgs)
{
	if (!ensure(InArgs._Controller.IsValid())) { return; }

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

	ChildSlot
	[
		SAssignNew(CommitList, SListView<TSharedPtr<FDiffHelperCommit>>)
		.ListItemsSource(&Commits)
		.OnGenerateRow(this, &SDiffHelperCommitPanel::OnGenerateRow)
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
			.FillWidth(0.2f)
			+ SHeaderRow::Column(SDiffHelperCommitPanelConstants::DateColumnId)
			.DefaultLabel(LOCTEXT("DateColumnLabel", "Date"))
			.FillWidth(0.25f)
		)
	];
}

SDiffHelperCommitPanel::~SDiffHelperCommitPanel()
{
	if (Controller.IsValid() && IsValid(Controller->GetModel()))
	{
		Controller->OnModelUpdated().RemoveAll(this);
	}
}

TSharedRef<ITableRow> SDiffHelperCommitPanel::OnGenerateRow(TSharedPtr<FDiffHelperCommit> InItem, const TSharedRef<STableViewBase>& InOwnerTable)
{
	return
		SNew(SDiffHelperCommitItem, InOwnerTable)
		.Item(InItem);
}

void SDiffHelperCommitPanel::OnModelUpdated()
{
	Commits = UDiffHelperUtils::ConvertToShared(Controller->GetModel()->SelectedDiffItem.Commits);
	CommitList->RequestListRefresh();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
#undef LOCTEXT_NAMESPACE
