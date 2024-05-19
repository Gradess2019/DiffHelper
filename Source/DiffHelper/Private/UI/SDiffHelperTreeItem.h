// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#include "DiffHelperTypes.h"

/**
 * 
 */
class DIFFHELPER_API SDiffHelperTreeItem : public STableRow<TSharedPtr<FDiffHelperTreeItem>>
{
public:
	SLATE_BEGIN_ARGS(SDiffHelperTreeItem)
		{
		}

		SLATE_ARGUMENT(TSharedPtr<FDiffHelperTreeItem>, Item)

	SLATE_END_ARGS()

protected:
	TSharedPtr<FDiffHelperTreeItem> Item;
	
public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwner);
};
