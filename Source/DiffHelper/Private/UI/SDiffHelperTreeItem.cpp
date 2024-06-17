// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperTreeItem.h"

#include "DiffHelperSettings.h"
#include "DiffHelperStyle.h"
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

	static const auto DefaultPadding = FMargin(4.f, 0.f, 0.f, 0.f);
	
	STableRow::Construct(
		STableRow::FArguments()
		.Content()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SImage)
				.Image(this, &SDiffHelperTreeItem::GetIconImage)
				.Visibility(this, &SDiffHelperTreeItem::GetIconVisibility)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(DefaultPadding)
			[
				Text.ToSharedRef()
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(DefaultPadding)
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

const FSlateBrush* SDiffHelperTreeItem::GetIconImage() const
{
	return FDiffHelperStyle::Get().GetBrush("DiffHelper.Directory.Small");
}

EVisibility SDiffHelperTreeItem::GetIconVisibility() const
{
	return Item->DiffItem.IsValid() ? EVisibility::Collapsed : EVisibility::HitTestInvisible;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

#undef LOCTEXT_NAMESPACE