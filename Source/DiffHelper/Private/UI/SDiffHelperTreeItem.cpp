// Copyright 2024 Gradess Games. All Rights Reserved.


#include "SDiffHelperTreeItem.h"

#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION


void SDiffHelperTreeItem::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwner)
{
	Item = InArgs._Item;
	if (!ensure(Item.IsValid())) { return; }

	STableRow::Construct(
		STableRow::FArguments()
		.Content()
		[
			SNew(STextBlock)
			.Text(FText::FromString(Item->Name))
		],
		InOwner
	);
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
