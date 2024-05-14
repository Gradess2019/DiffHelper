// Copyright 2024 Gradess Games. All Rights Reserved.


#include "UI/SDiffHelperBranchPicker.h"

#include "DiffHelperUtils.h"
#include "SlateOptMacros.h"

#include "UI/DiffHelperTabController.h"
#include "UI/DiffHelperTabModel.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SDiffHelperBranchPicker::Construct(const FArguments& InArgs)
{
	Controller = InArgs._Controller;
	Hint = InArgs._Hint;

	ensure(Controller.IsValid());

	if (!InArgs._Options)
	{
		const auto* Model = Controller->GetModel();
		if (ensure(IsValid(Model)))
		{
			const auto Branches = UDiffHelperUtils::ConvertBranchesToStringArray(Model->Branches);
			Options = MakeShared<const TArray<TSharedPtr<FString>>>(UDiffHelperUtils::ConvertToShared(Branches));
		}
	}
	else
	{
		Options = MakeShared<TArray<TSharedPtr<FString>>>(*InArgs._Options);
	}

	SelectedBranch = InArgs._InitiallySelectedBranch.IsValid() ? *InArgs._InitiallySelectedBranch : FDiffHelperBranch();
	TSharedPtr<FString> InitiallySelectedItem = SelectedBranch.IsValid() ? MakeShareable(new FString(SelectedBranch.Name)) : nullptr;

	SSearchableComboBox::Construct(
		SSearchableComboBox::FArguments()
		.OptionsSource(Options.Get())
		.InitiallySelectedItem(InitiallySelectedItem)
		.OnGenerateWidget(this, &SDiffHelperBranchPicker::HandleGenerateWidget)
		.OnSelectionChanged(this, &SDiffHelperBranchPicker::HandleSelectionChanged)
		.Content()
		[
			SNew(STextBlock).Text(this, &SDiffHelperBranchPicker::GetSelectedItemText)
		]
	);
}

TSharedRef<SWidget> SDiffHelperBranchPicker::HandleGenerateWidget(TSharedPtr<FString> String)
{
	return SNew(STextBlock).Text(FText::FromString(*String));
}

void SDiffHelperBranchPicker::HandleSelectionChanged(TSharedPtr<FString, ESPMode::ThreadSafe> String, ESelectInfo::Type Arg)
{
	if (!ensure(String.IsValid()) || !ensure(Controller.IsValid())) { return; }

	const auto& Branches = Controller->GetModel()->Branches;
	const auto* FoundBranch = Branches.FindByPredicate([String](const FDiffHelperBranch& Branch)
	{
		return Branch.Name == *String;
	});

	if (!FoundBranch)
	{
		UE_LOG(LogDiffHelper, Error, TEXT("Branch %s not found in the model"), **String);
	}

	SelectedBranch = *FoundBranch;
}

FText SDiffHelperBranchPicker::GetSelectedItemText() const
{
	if (SelectedBranch.IsValid())
	{
		return FText::FromString(SelectedBranch.Name);
	}

	return Hint;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
