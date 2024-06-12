// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperTreeItem.h"

#include "DiffHelperSettings.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION


void SDiffHelperTreeItem::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwner)
{
	Item = InArgs._Item;
	if (!ensure(Item.IsValid())) { return; }

	Text = SNew(STextBlock)
		.Text(FText::FromString(Item->Name));

	if (Item->DiffItem.IsValid())
	{
		const auto& StatusColor = GetDefault<UDiffHelperSettings>()->StatusColors.FindRef(Item->DiffItem->Status, FLinearColor::White);
		Text->SetColorAndOpacity(StatusColor);
	}
	
	STableRow::Construct(
		STableRow::FArguments()
		.Content()
		[
			Text.ToSharedRef()
		],
		InOwner
	);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
