// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UDiffHelperTabController;
class UDiffHelperTabModel;
struct FDiffHelperItemNode;
/**
 * 
 */
class DIFFHELPER_API SDiffHelperDiffPanelList : public SListView<TSharedPtr<FDiffHelperItemNode>>
{
public:
	DECLARE_DELEGATE_RetVal_OneParam(bool, FCanBroadcastSelectionChanged, const TSharedPtr<SListView>&);
	
public:
	SLATE_BEGIN_ARGS(SDiffHelperDiffPanelList)
		: _Controller(nullptr)
		{
		}

		SLATE_ARGUMENT(TWeakObjectPtr<UDiffHelperTabController>, Controller)

		SLATE_ATTRIBUTE(EColumnSortMode::Type, SortMode)
		
		SLATE_EVENT(FOnSelectionChanged, OnSelectionChanged)
		SLATE_EVENT(FOnGenerateRow, OnGenerateRow)
		SLATE_EVENT(FOnSortModeChanged, OnSortModeChanged)
		SLATE_EVENT(FOnContextMenuOpening, OnContextMenuOpening)
		SLATE_EVENT(FCanBroadcastSelectionChanged, CanBroadcastSelectionChanged)

	SLATE_END_ARGS()

private:
	TWeakObjectPtr<UDiffHelperTabController> Controller;
	TWeakObjectPtr<const UDiffHelperTabModel> Model;

	FCanBroadcastSelectionChanged CanBroadcastSelectionChanged;

	
public:
	void Construct(const FArguments& InArgs);

protected:
	virtual void Private_SignalSelectionChanged(ESelectInfo::Type SelectInfo) override;
};
