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

	virtual ~SDiffHelperWindow() override;

protected:
	TObjectPtr<UDiffHelperTabController> Controller;

public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
	
};
