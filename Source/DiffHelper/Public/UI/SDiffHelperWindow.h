// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class UDiffHelperTabController;

class DIFFHELPER_API SDiffHelperWindow : public SWindow
{
public:
	SLATE_BEGIN_ARGS(SDiffHelperWindow)
		{
		}

	SLATE_END_ARGS()

public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

protected:
	TSharedRef<SWidget> CreateStartupView();
	TSharedRef<SWidget> CreateMenu();
	TSharedRef<SWidget> CreateDiffPicker();
	void Reset();
};
