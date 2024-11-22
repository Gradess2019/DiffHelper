// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DiffHelperTypes.h"

#include "Widgets/SCompoundWidget.h"

class UDiffHelperTabController;

class DIFFHELPER_API SDiffHelperDiffViewer : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDiffHelperDiffViewer)
		{
		}

		SLATE_ARGUMENT(const FDiffHelperBranch*, SourceBranch)
		SLATE_ARGUMENT(const FDiffHelperBranch*, TargetBranch)

	SLATE_END_ARGS()

	virtual ~SDiffHelperDiffViewer() override;

protected:
	TWeakObjectPtr<UDiffHelperTabController> Controller = nullptr;

public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
};
