﻿// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperTreeItem.h"

#include "DiffHelperSettings.h"
#include "DiffHelperUtils.h"
#include "SlateOptMacros.h"


#define LOCTEXT_NAMESPACE "DiffHelper"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION


void SDiffHelperTreeItem::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwner)
{
	Item = InArgs._Item;
	if (!ensure(Item.IsValid())) { return; }

	Text = SNew(STextBlock)
		.Text(FText::FromString(Item->Name));

	const auto* Settings = GetDefault<UDiffHelperSettings>();
	Hint = SNew(STextBlock);
	Hint->SetColorAndOpacity(Settings->ItemHintColor);
	
	if (Item->DiffItem.IsValid())
	{
		ShowFileHint();
	}
	else
	{
		ShowDirectoryHint();
	}
	
	STableRow::Construct(
		STableRow::FArguments()
		.Content()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				Text.ToSharedRef()
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(4.f, 0.f, 0.f, 0.f)
			[
				Hint.ToSharedRef()
			]
		],
		InOwner
	);
}

void SDiffHelperTreeItem::ShowFileHint()
{
	const auto* Settings = GetDefault<UDiffHelperSettings>();
	const auto& StatusColor = Settings->StatusColors.FindRef(Item->DiffItem->Status, FLinearColor::White);
	Text->SetColorAndOpacity(StatusColor);

	auto PathWithoutFilename = FPaths::GetPath(Item->Path).TrimStartAndEnd();
	PathWithoutFilename.TrimCharInline(TEXT('/'), nullptr);
	PathWithoutFilename.TrimCharInline(TEXT('\\'), nullptr);
	
	Hint->SetText(FText::FromString(PathWithoutFilename));
}

void SDiffHelperTreeItem::ShowDirectoryHint()
{
	const auto& AllChildrenCount = UDiffHelperUtils::GetItemNodeFilesCount(Item);
	Hint->SetText(FText::Format(LOCTEXT("TreeItemFilesCount", "{0} files"), AllChildrenCount));
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE