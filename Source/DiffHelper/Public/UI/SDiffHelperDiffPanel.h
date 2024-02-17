// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class DIFFHELPER_API SDiffHelperDiffPanel : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDiffHelperDiffPanel)
		{
		}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
};
