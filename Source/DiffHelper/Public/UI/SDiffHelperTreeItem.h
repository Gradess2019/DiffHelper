// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#include "DiffHelperTypes.h"

class UDiffHelperTabController;
class STextBlock;

/**
 * 
 */
class DIFFHELPER_API SDiffHelperTreeItem : public STableRow<TSharedPtr<FDiffHelperItemNode>>
{
public:
	SLATE_BEGIN_ARGS(SDiffHelperTreeItem)
		: _Controller(nullptr)
		{
		}

		SLATE_ARGUMENT(TWeakObjectPtr<UDiffHelperTabController>, Controller)
		SLATE_ARGUMENT(TSharedPtr<FDiffHelperItemNode>, Item)

	SLATE_END_ARGS()

protected:
	TSharedPtr<STextBlock> Text;
	TSharedPtr<STextBlock> Hint;
	TSharedPtr<FDiffHelperItemNode> Item;

	TWeakObjectPtr<UDiffHelperTabController> Controller;
	
public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwner);
	
private:
	void ShowFileHint();
	void ShowDirectoryHint();

	FSlateColor GetTextColor() const;
	FLinearColor GetTextHighlightColor() const;
	FText GetHighlightText() const;
	const FSlateBrush* GetBackgroundHighlightColor() const;
	const FSlateBrush* GetIconImage() const;
	EVisibility GetIconVisibility() const;
	
};
