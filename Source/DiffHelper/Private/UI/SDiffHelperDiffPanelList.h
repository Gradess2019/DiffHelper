﻿// Copyright 2024 Gradess Games. All Rights Reserved.

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
	SLATE_BEGIN_ARGS(SDiffHelperDiffPanelList)
		: _Controller(nullptr)
		{
		}

		SLATE_ARGUMENT(TWeakObjectPtr<UDiffHelperTabController>, Controller)

	SLATE_END_ARGS()

private:
	TWeakObjectPtr<UDiffHelperTabController> Controller;
	TWeakObjectPtr<const UDiffHelperTabModel> Model;

	
public:
	void Construct(const FArguments& InArgs);
};
