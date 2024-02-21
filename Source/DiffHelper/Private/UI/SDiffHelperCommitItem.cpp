// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperCommitItem.h"
#include "SDiffHelperCommitTextBlock.h"
#include "SlateOptMacros.h"

#define LOCTEXT_NAMESPACE "DiffHelper"
BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperCommitItem::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTable)
{
	if (!ensure(InArgs._Item.IsValid())) { return; }

	Item = InArgs._Item;


	FSuperRowType::Construct(
		FSuperRowType::FArguments()
		.Style(&FAppStyle::Get().GetWidgetStyle<FTableRowStyle>("PropertyTable.TableRow")),
		InOwnerTable
	);
}

TSharedRef<SWidget> SDiffHelperCommitItem::GenerateWidgetForColumn(const FName& InColumnName)
{
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
	else if (InColumnName == SDiffHelperCommitPanelConstants::DiffButtonColumnId)
	{
		return CreateDiffButtonColumn();
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

TSharedRef<SWidget> SDiffHelperCommitItem::CreateDiffButtonColumn() const
{
	return
		SNew(SButton)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("DiffButtonText", "Diff"))
			.Justification(ETextJustify::Center)
		];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
#undef LOCTEXT_NAMESPACE
