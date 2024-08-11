// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperCommitItem.h"

#include "DiffHelperSettings.h"

#include "UI/DiffHelperTabController.h"
#include "UI/DiffHelperTabModel.h"
#include "UI/SDiffHelperCommitTextBlock.h"
#include "SlateOptMacros.h"

#define LOCTEXT_NAMESPACE "DiffHelper"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperCommitItem::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTable)
{
	if (!ensure(InArgs._Controller.IsValid()) || !ensure(InArgs._Item.IsValid())) { return; }

	Controller = InArgs._Controller;
	Item = InArgs._Item;

	FSuperRowType::Construct(
		FSuperRowType::FArguments()
		.Style(&FAppStyle::Get().GetWidgetStyle<FTableRowStyle>("PropertyTable.TableRow")),
		InOwnerTable
	);

	const auto& DiffItem = Controller->GetModel()->SelectedDiffItem;
	const auto* StatusInCommit = Item->Files.FindByPredicate([&DiffItem](const FDiffHelperFileData& File)
	{
		return File.Path == DiffItem.Path;
	});

	if (StatusInCommit)
	{
		const auto* Settings = GetDefault<UDiffHelperSettings>();
		const auto& StatusColor = Settings->StatusColors.FindRef(StatusInCommit->Status, FLinearColor::White);
		
		SetForegroundColor(StatusColor);
	}
}

TSharedRef<SWidget> SDiffHelperCommitItem::GenerateWidgetForColumn(const FName& InColumnName)
{
	if (!ensure(Controller.IsValid()) || !ensure(Item.IsValid())) { return SNullWidget::NullWidget; }
	
	if (InColumnName == SDiffHelperCommitPanelConstants::HashColumnId)
	{
		return CreateCommitHashColumn();
	}
	else if (InColumnName == SDiffHelperCommitPanelConstants::MessageColumnId)
	{
		return CreateMessageColumn();
	}
	else if (InColumnName == SDiffHelperCommitPanelConstants::AuthorColumnId)
	{
		return CreateAuthorColumn();
	}
	else if (InColumnName == SDiffHelperCommitPanelConstants::DateColumnId)
	{
		return CreateDateColumn();
	}

	ensureMsgf(false, TEXT("Unknown column name %s"), *InColumnName.ToString());
	return SNullWidget::NullWidget;
}

TSharedRef<SWidget> SDiffHelperCommitItem::CreateCommitHashColumn() const
{
	return
		SNew(SDiffHelperCommitTextBlock)
		.Text(FText::FromString(Item->Revision))
		.ToolTip(FText::FromString(Item->Revision));
}

TSharedRef<SWidget> SDiffHelperCommitItem::CreateMessageColumn() const
{
	return
		SNew(SDiffHelperCommitTextBlock)
		.Text(FText::FromString(Item->Message))
		.ToolTip(FText::FromString(Item->Message));
}

TSharedRef<SWidget> SDiffHelperCommitItem::CreateAuthorColumn() const
{
	return
		SNew(SDiffHelperCommitTextBlock)
		.Text(FText::FromString(Item->Author))
		.ToolTip(FText::FromString(Item->Author));
}

TSharedRef<SWidget> SDiffHelperCommitItem::CreateDateColumn() const
{
	return
		SNew(SDiffHelperCommitTextBlock)
		.Text(FText::FromString(Item->Date.ToString()))
		.ToolTip(FText::FromString(Item->Date.ToString()));
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
#undef LOCTEXT_NAMESPACE
