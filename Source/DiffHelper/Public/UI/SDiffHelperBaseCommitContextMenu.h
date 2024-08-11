// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UDiffHelperTabController;
/**
 * 
 */
class DIFFHELPER_API SDiffHelperBaseCommitContextMenu : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDiffHelperBaseCommitContextMenu)
		{
		}

		SLATE_ARGUMENT(TWeakObjectPtr<UDiffHelperTabController>, Controller)

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

protected:
	virtual void BuildMenu(FMenuBuilder& InMenuBuilder) = 0;
};
