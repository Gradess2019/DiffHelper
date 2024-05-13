// Copyright 2024 Gradess Games. All Rights Reserved.


#include "SDiffHelperDiffFileItem.h"

#include "DiffHelper.h"
#include "DiffHelperManager.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperDiffFileItem::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwner)
{
	Item = InArgs._Item;
	if (!ensure(Item.IsValid())) { return; }

	FSuperRowType::Construct(
		FSuperRowType::FArguments()
		.Style(&FAppStyle::Get().GetWidgetStyle<FTableRowStyle>("PropertyTable.TableRow")),
		InOwner
	);
}

TSharedRef<SWidget> SDiffHelperDiffFileItem::GenerateWidgetForColumn(const FName& InColumnName)
{
	if (InColumnName.IsEqual(SDiffHelperDiffPanelConstants::StatusColumnId))
	{
		return CreateStatusColumn();
	}
	else if (InColumnName.IsEqual(SDiffHelperDiffPanelConstants::PathColumnId))
	{
		return CreatePathColumn();
	}

	ensureMsgf(false, TEXT("Unknown column name"));
	return SNullWidget::NullWidget;
}

TSharedRef<SWidget> SDiffHelperDiffFileItem::CreateStatusColumn() const
{
	if (Item->Commits.Num() > 0)
	{
		const auto* FileDataPtr = Item->Commits[0].Files.FindByPredicate([this](const FDiffHelperFileData& FileData) { return FileData.Path == Item->Path; });
		if (FileDataPtr)
		{
			const auto& FileData = *FileDataPtr;
			const auto StatusIcon = FDiffHelperModule::Get().GetManager()->GetStatusIcon(FileData.Status);
			return
				SNew(SOverlay)
				+ SOverlay::Slot()
				  .HAlign(HAlign_Center)
				  .VAlign(VAlign_Center)
				[
					SNew(SImage)
					.Image(StatusIcon.GetIcon())
				];
		}
	}

	return SNew(SImage);
}

TSharedRef<SWidget> SDiffHelperDiffFileItem::CreatePathColumn() const
{
	return SNew(STextBlock).Text(FText::FromString(Item->Path));
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
