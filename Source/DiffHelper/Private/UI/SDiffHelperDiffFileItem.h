// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>
#include <Widgets/SCompoundWidget.h>

#include "DiffHelperTypes.h"

class DIFFHELPER_API SDiffHelperDiffFileItem : public SMultiColumnTableRow<TSharedPtr<FDiffHelperDiffItem>>
{
public:
	SLATE_BEGIN_ARGS(SDiffHelperDiffFileItem)
		:
		_Item(nullptr)
		{}
	
	SLATE_ARGUMENT(TSharedPtr<FDiffHelperDiffItem>, Item)

	SLATE_END_ARGS()

protected:
	TSharedPtr<FDiffHelperDiffItem> Item;

public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwner);
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& InColumnName) override;

protected:
	TSharedRef<SWidget> CreateStatusColumn() const;
	TSharedRef<SWidget> CreatePathColumn() const;
};
