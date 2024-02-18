// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

struct FDiffHelperDiffItem;
class UDiffHelperTabController;

class DIFFHELPER_API SDiffHelperDiffPanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDiffHelperDiffPanel)
			:
			_Controller(nullptr)
		{
		}

		SLATE_ARGUMENT(TWeakObjectPtr<UDiffHelperTabController>, Controller)

	SLATE_END_ARGS()

protected:
	TWeakObjectPtr<UDiffHelperTabController> Controller;
	TArray<TSharedPtr<FDiffHelperDiffItem>> Diff;

public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

protected:
	void OnSearchTextChanged(const FText& Text);
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FDiffHelperDiffItem> Item, const TSharedRef<STableViewBase>& OwnerTable);
};
