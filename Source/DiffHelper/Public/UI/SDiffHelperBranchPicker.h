// Copyright 2024 Gradess Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DiffHelperTypes.h"
#include "SSearchableComboBox.h"

class UDiffHelperTabController;

class DIFFHELPER_API SDiffHelperBranchPicker : public SSearchableComboBox
{
public:
	SLATE_BEGIN_ARGS(SDiffHelperBranchPicker)
			:
			_Controller(nullptr),
			_Options(nullptr)
		{
		}

		SLATE_ARGUMENT(TWeakObjectPtr<UDiffHelperTabController>, Controller)
		SLATE_ARGUMENT(TArray<TSharedPtr<FString>>*, Options)

	SLATE_END_ARGS()

protected:
	TWeakObjectPtr<UDiffHelperTabController> Controller = nullptr;
	TSharedPtr<const TArray<TSharedPtr<FString>>> Options = nullptr;
	
	FDiffHelperBranch SelectedBranch;

public:
	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

protected:
	TSharedRef<SWidget> HandleGenerateWidget(TSharedPtr<FString> String);
	void HandleSelectionChanged(TSharedPtr<FString, ESPMode::ThreadSafe> String, ESelectInfo::Type Arg);

	FText GetSelectedItemText() const;
};
