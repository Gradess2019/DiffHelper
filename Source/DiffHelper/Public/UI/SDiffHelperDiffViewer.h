// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class DIFFHELPER_API SDiffHelperDiffViewer : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDiffHelperDiffViewer)
		{
		}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
};
