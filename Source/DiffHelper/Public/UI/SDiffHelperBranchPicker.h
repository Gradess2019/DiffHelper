// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SSearchableComboBox.h"

#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class DIFFHELPER_API SDiffHelperBranchPicker : public SSearchableComboBox
{
public:
	SLATE_BEGIN_ARGS(SDiffHelperBranchPicker)
		{
		}

	SLATE_END_ARGS()

protected:
	TArray<TSharedPtr<FString>> Options;

public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

protected:
	TSharedRef<SWidget> HandleGenerateWidget(TSharedPtr<FString> String);
	void HandleSelectionChanged(TSharedPtr<FString, ESPMode::ThreadSafe> String, ESelectInfo::Type Arg);
};
