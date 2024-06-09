// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#include "DiffHelperTypes.h"

/**
 * 
 */
class DIFFHELPER_API SDiffHelperTreeItem : public STableRow<TSharedPtr<FDiffHelperItemNode>>
{
public:
	SLATE_BEGIN_ARGS(SDiffHelperTreeItem)
		{
		}

		SLATE_ARGUMENT(TSharedPtr<FDiffHelperItemNode>, Item)

	SLATE_END_ARGS()

protected:
	TSharedPtr<FDiffHelperItemNode> Item;
	
public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwner);
};
